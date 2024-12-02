from fastapi import Request
from ..security.attack_analyzer import attack_analyzer
from ..security.logging import security_logger, SecurityEventType
import asyncio

async def security_middleware(request: Request, call_next):
    # Получаем IP-адрес
    ip_address = request.client.host
    
    # Анализируем IP
    is_suspicious = await attack_analyzer.analyze_ip_pattern(ip_address)
    if is_suspicious:
        await security_logger.log_security_event(
            SecurityEventType.SUSPICIOUS_IP,
            {
                "ip_address": ip_address,
                "path": request.url.path,
                "method": request.method
            },
            ip_address,
            severity="warning"
        )
    
    # Анализируем паттерн запросов
    await attack_analyzer.analyze_request_pattern(
        ip_address,
        request.url.path,
        request.method
    )
    
    # Продолжаем обработку запроса
    response = await call_next(request)
    
    # Логируем неуспешные запросы
    if response.status_code >= 400:
        await security_logger.log_security_event(
            SecurityEventType.UNAUTHORIZED_ACCESS,
            {
                "ip_address": ip_address,
                "path": request.url.path,
                "method": request.method,
                "status_code": response.status_code
            },
            ip_address,
            severity="warning" if response.status_code >= 500 else "info"
        )
    
    return response 