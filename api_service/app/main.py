from fastapi import FastAPI, Request
from fastapi.middleware.cors import CORSMiddleware
from fastapi.openapi.docs import get_swagger_ui_html
from fastapi.responses import JSONResponse
import asyncio
import uvicorn
from .config import settings
from .routers import sensors, websocket, security
from .kafka_client import KafkaClient
from .docs.description import API_DESCRIPTION, tags_metadata
from .security.rate_limit import RateLimitMiddleware
from .middleware.security import security_middleware
from .security.logging import security_logger, SecurityEventType
from .security.exceptions import SecurityException

app = FastAPI(
    title=settings.APP_NAME,
    version=settings.VERSION,
    description=API_DESCRIPTION,
    openapi_tags=tags_metadata,
    docs_url=None,
    redoc_url=None
)

# CORS middleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=settings.CORS_ORIGINS,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Error handling
@app.exception_handler(Exception)
async def global_exception_handler(request: Request, exc: Exception):
    return JSONResponse(
        status_code=500,
        content={
            "success": False,
            "message": "Internal server error",
            "detail": str(exc)
        }
    )

# Startup and shutdown events
@app.on_event("startup")
async def startup_event():
    await sensors.kafka_client.start()

@app.on_event("shutdown")
async def shutdown_event():
    await sensors.kafka_client.stop()

# Custom documentation endpoints
@app.get("/docs", include_in_schema=False)
async def custom_swagger_ui_html():
    return get_swagger_ui_html(
        openapi_url=f"{settings.API_V1_PREFIX}/openapi.json",
        title=f"{settings.APP_NAME} - Swagger UI",
        oauth2_redirect_url=f"{settings.API_V1_PREFIX}/docs/oauth2-redirect",
        swagger_js_url="/static/swagger-ui-bundle.js",
        swagger_css_url="/static/swagger-ui.css",
    )

# Include routers
app.include_router(
    sensors.router,
    prefix=settings.API_V1_PREFIX,
    tags=["sensors"]
)

app.include_router(
    websocket.router,
    prefix=settings.API_V1_PREFIX,
    tags=["websocket"]
)

app.include_router(
    security.router,
    prefix=f"{settings.API_V1_PREFIX}/security",
    tags=["security"]
)

# Kafka consumer для WebSocket broadcast
@app.on_event("startup")
async def start_kafka_consumer():
    async def process_message(message):
        sensor_data = SensorData(**message)
        await manager.broadcast_sensor_data(sensor_data)
    
    await sensors.kafka_client.start()
    asyncio.create_task(
        sensors.kafka_client.consume_messages(process_message)
    )

# Добавляем rate limiting middleware
app.add_middleware(RateLimitMiddleware)

# Обновляем обработчик ошибок
@app.exception_handler(429)
async def rate_limit_handler(request: Request, exc):
    return JSONResponse(
        status_code=429,
        content={
            "detail": "Too many requests. Please try again later."
        }
    )

# Добавляем middleware безопасности
app.middleware("http")(security_middleware)

# Обработчик исключений безопасности
@app.exception_handler(SecurityException)
async def security_exception_handler(request: Request, exc: SecurityException):
    await security_logger.log_security_event(
        exc.event_type,
        exc.details,
        request.client.host,
        severity="critical"
    )
    return JSONResponse(
        status_code=exc.status_code,
        content={"detail": exc.detail}
    )

if __name__ == "__main__":
    uvicorn.run(
        "app.main:app",
        host="0.0.0.0",
        port=8000,
        reload=settings.DEBUG
    ) 