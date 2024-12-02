from typing import Optional, Dict, Any
import aiohttp
from ..config import settings
import json
from enum import Enum
import asyncio

class AlertSeverity(Enum):
    INFO = "info"
    WARNING = "warning" 
    CRITICAL = "critical"

class AlertManager:
    def __init__(self):
        self.slack_webhook = settings.SLACK_WEBHOOK_URL
        self.telegram_bot_token = settings.TELEGRAM_BOT_TOKEN
        self.telegram_chat_id = settings.TELEGRAM_CHAT_ID

    async def send_alert(
        self,
        title: str,
        description: str,
        severity: str,
        additional_data: Optional[Dict[str, Any]] = None
    ):
        """Отправка алертов в различные каналы"""
        if severity in [AlertSeverity.WARNING.value, AlertSeverity.CRITICAL.value]:
            tasks = []
            
            if self.slack_webhook:
                tasks.append(self.send_slack_alert(
                    title, description, severity, additional_data
                ))
            
            if self.telegram_bot_token and self.telegram_chat_id:
                tasks.append(self.send_telegram_alert(
                    title, description, severity, additional_data
                ))
            
            if tasks:
                await asyncio.gather(*tasks)

    async def send_slack_alert(
        self,
        title: str,
        description: str,
        severity: str,
        additional_data: Optional[Dict[str, Any]] = None
    ):
        """Отправка алерта в Slack"""
        color = {
            "info": "#36a64f",
            "warning": "#ffcc00", 
            "critical": "#ff0000"
        }.get(severity, "#36a64f")

        payload = {
            "attachments": [{
                "color": color,
                "title": title,
                "text": description,
                "fields": [
                    {"title": "Severity", "value": severity, "short": True}
                ]
            }]
        }

        if additional_data:
            payload["attachments"][0]["fields"].extend([
                {"title": k, "value": str(v), "short": True}
                for k, v in additional_data.items()
            ])

        try:
            async with aiohttp.ClientSession() as session:
                async with session.post(self.slack_webhook, json=payload) as response:
                    if response.status != 200:
                        print(f"Failed to send Slack alert: {await response.text()}")
        except Exception as e:
            print(f"Error sending Slack alert: {e}")

    async def send_telegram_alert(
        self,
        title: str,
        description: str,
        severity: str,
        additional_data: Optional[Dict[str, Any]] = None
    ):
        """Отправка алерта в Telegram"""
        emoji = {
            "info": "ℹ️",
            "warning": "⚠️",
            "critical": "🚨"
        }.get(severity, "ℹ️")

        message = f"{emoji} *{title}*\n\n{description}"

        if additional_data:
            message += "\n\n*Additional Information:*\n"
            message += "\n".join(f"- {k}: {v}" for k, v in additional_data.items())

        url = f"https://api.telegram.org/bot{self.telegram_bot_token}/sendMessage"
        payload = {
            "chat_id": self.telegram_chat_id,
            "text": message,
            "parse_mode": "Markdown"
        }

        try:
            async with aiohttp.ClientSession() as session:
                async with session.post(url, json=payload) as response:
                    if response.status != 200:
                        print(f"Failed to send Telegram alert: {await response.text()}")
        except Exception as e:
            print(f"Error sending Telegram alert: {e}")

alert_manager = AlertManager() 