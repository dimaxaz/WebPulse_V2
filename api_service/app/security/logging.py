from datetime import datetime
import json
import logging
from elasticsearch import AsyncElasticsearch
from typing import Dict, Any, Optional
from ..config import settings
import structlog
from enum import Enum

class SecurityEventType(Enum):
    LOGIN_ATTEMPT = "login_attempt"
    BRUTE_FORCE = "brute_force"
    RATE_LIMIT = "rate_limit"
    API_KEY_INVALID = "api_key_invalid"
    TWO_FACTOR_FAIL = "two_factor_fail"
    UNAUTHORIZED_ACCESS = "unauthorized_access"
    SUSPICIOUS_IP = "suspicious_ip"

class SecurityLogger:
    def __init__(self):
        # Настройка структурированного логирования
        structlog.configure(
            processors=[
                structlog.processors.TimeStamper(fmt="iso"),
                structlog.processors.JSONRenderer()
            ]
        )
        self.logger = structlog.get_logger()
        
        # Elasticsearch для хранения логов
        self.es = AsyncElasticsearch([
            f"http://{settings.ELASTICSEARCH_HOST}:{settings.ELASTICSEARCH_PORT}"
        ])
        
    async def log_security_event(
        self,
        event_type: SecurityEventType,
        details: Dict[str, Any],
        ip_address: Optional[str] = None,
        user_id: Optional[int] = None,
        severity: str = "info"
    ):
        event = {
            "timestamp": datetime.utcnow().isoformat(),
            "event_type": event_type.value,
            "details": details,
            "ip_address": ip_address,
            "user_id": user_id,
            "severity": severity
        }
        
        # Логируем локально
        self.logger.info("security_event", **event)
        
        # Сохраняем в Elasticsearch
        await self.es.index(
            index=f"security-logs-{datetime.utcnow():%Y-%m}",
            document=event
        )
        
        # Проверяем необходимость алерта
        if severity in ["warning", "critical"]:
            await self.trigger_alert(event)
    
    async def trigger_alert(self, event: Dict[str, Any]):
        # Отправляем алерт в систему мониторинга
        await alert_manager.send_alert(
            title=f"Security Alert: {event['event_type']}",
            description=json.dumps(event['details'], indent=2),
            severity=event['severity']
        )

security_logger = SecurityLogger() 