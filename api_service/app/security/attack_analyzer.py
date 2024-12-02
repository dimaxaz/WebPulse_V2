from datetime import datetime, timedelta
from typing import Dict, List, Set
import aioredis
import ipaddress
from ..config import settings
import geoip2.database
import asyncio

class AttackAnalyzer:
    def __init__(self):
        self.redis = aioredis.from_url(
            f"redis://{settings.REDIS_HOST}:{settings.REDIS_PORT}/{settings.REDIS_DB_SECURITY}"
        )
        self.geoip_reader = geoip2.database.Reader('GeoLite2-City.mmdb')
        
    async def analyze_login_attempts(self, ip_address: str, user_id: int):
        """Анализ попыток входа"""
        key = f"login_attempts:{ip_address}:{user_id}"
        
        # Увеличиваем счетчик попыток
        attempts = await self.redis.incr(key)
        await self.redis.expire(key, 3600)  # TTL 1 час
        
        if attempts >= settings.MAX_LOGIN_ATTEMPTS:
            await security_logger.log_security_event(
                SecurityEventType.BRUTE_FORCE,
                {
                    "ip_address": ip_address,
                    "user_id": user_id,
                    "attempts": attempts
                },
                ip_address,
                user_id,
                "critical"
            )
            return True
        return False
    
    async def analyze_ip_pattern(self, ip_address: str) -> bool:
        """Анализ паттернов IP-адресов"""
        try:
            # Проверяем геолокацию
            response = self.geoip_reader.city(ip_address)
            country = response.country.iso_code
            
            # Проверяем, является ли IP подозрительным
            if country in settings.SUSPICIOUS_COUNTRIES:
                await security_logger.log_security_event(
                    SecurityEventType.SUSPICIOUS_IP,
                    {
                        "ip_address": ip_address,
                        "country": country
                    },
                    ip_address,
                    severity="warning"
                )
                return True
                
            # Проверяем, является ли IP частью известной ботнет-сети
            if await self.check_botnet_ip(ip_address):
                return True
                
        except Exception as e:
            await security_logger.log_security_event(
                SecurityEventType.SUSPICIOUS_IP,
                {
                    "ip_address": ip_address,
                    "error": str(e)
                },
                ip_address,
                severity="warning"
            )
        return False
    
    async def check_botnet_ip(self, ip_address: str) -> bool:
        """Проверка IP в списке известных ботнетов"""
        # Здесь можно добавить интеграцию с внешними сервисами
        # проверки репутации IP
        return False
    
    async def analyze_request_pattern(
        self,
        ip_address: str,
        endpoint: str,
        method: str
    ):
        """Анализ паттернов запросов"""
        key = f"request_pattern:{ip_address}"
        
        # Сохраняем паттерн запроса
        await self.redis.lpush(key, f"{method}:{endpoint}")
        await self.redis.ltrim(key, 0, settings.MAX_REQUEST_PATTERNS - 1) 