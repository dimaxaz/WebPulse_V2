from fastapi import HTTPException, Request
import redis
from datetime import datetime
import time
from ..config import settings
from typing import Optional

class RateLimiter:
    def __init__(self):
        self.redis = redis.Redis(
            host=settings.REDIS_HOST,
            port=settings.REDIS_PORT,
            db=settings.REDIS_DB
        )
        
    async def check_rate_limit(
        self,
        key: str,
        max_requests: int,
        window_seconds: int
    ) -> bool:
        pipe = self.redis.pipeline()
        now = time.time()
        window_start = now - window_seconds
        
        # Очищаем старые запросы
        pipe.zremrangebyscore(key, 0, window_start)
        # Добавляем текущий запрос
        pipe.zadd(key, {str(now): now})
        # Подсчитываем запросы в окне
        pipe.zcard(key)
        # Устанавливаем TTL для ключа
        pipe.expire(key, window_seconds)
        
        results = pipe.execute()
        request_count = results[2]
        
        return request_count <= max_requests

class RateLimitMiddleware:
    def __init__(
        self,
        max_requests: int = 100,
        window_seconds: int = 60
    ):
        self.limiter = RateLimiter()
        self.max_requests = max_requests
        self.window_seconds = window_seconds
    
    async def __call__(self, request: Request, call_next):
        client_ip = request.client.host
        key = f"rate_limit:{client_ip}"
        
        if not await self.limiter.check_rate_limit(
            key,
            self.max_requests,
            self.window_seconds
        ):
            raise HTTPException(
                status_code=429,
                detail="Too many requests"
            )
        
        return await call_next(request) 