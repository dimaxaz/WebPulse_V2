from fastapi import Security, HTTPException
from fastapi.security.api_key import APIKeyHeader
from typing import Optional
import redis
import secrets
from ..config import settings

api_key_header = APIKeyHeader(name="X-API-Key", auto_error=False)

class APIKeyManager:
    def __init__(self):
        self.redis = redis.Redis(
            host=settings.REDIS_HOST,
            port=settings.REDIS_PORT,
            db=settings.REDIS_DB_API_KEYS
        )
    
    def generate_api_key(self, user_id: int, scopes: list[str]) -> str:
        api_key = secrets.token_urlsafe(32)
        key_data = {
            "user_id": user_id,
            "scopes": ",".join(scopes)
        }
        self.redis.hmset(f"api_key:{api_key}", key_data)
        return api_key
    
    def validate_api_key(self, api_key: str) -> Optional[dict]:
        key_data = self.redis.hgetall(f"api_key:{api_key}")
        if not key_data:
            return None
        
        return {
            "user_id": int(key_data[b"user_id"]),
            "scopes": key_data[b"scopes"].decode().split(",")
        }
    
    def revoke_api_key(self, api_key: str):
        self.redis.delete(f"api_key:{api_key}")

api_key_manager = APIKeyManager()

async def get_api_key(
    api_key: str = Security(api_key_header)
) -> dict:
    if api_key is None:
        raise HTTPException(
            status_code=403,
            detail="Could not validate API key"
        )
    
    key_data = api_key_manager.validate_api_key(api_key)
    if key_data is None:
        raise HTTPException(
            status_code=403,
            detail="Invalid API key"
        )
    
    return key_data 