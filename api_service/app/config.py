from pydantic_settings import BaseSettings
from typing import List, Optional

class Settings(BaseSettings):
    APP_NAME: str = "Sensor API Service"
    DEBUG: bool = False
    VERSION: str = "1.0.0"
    
    # Kafka settings
    KAFKA_BOOTSTRAP_SERVERS: List[str] = ["localhost:9092"]
    KAFKA_SENSOR_TOPIC: str = "sensor_data"
    KAFKA_CONSUMER_GROUP: str = "api_service_group"
    
    # API settings
    API_V1_PREFIX: str = "/api/v1"
    CORS_ORIGINS: List[str] = ["*"]
    
    # Performance settings
    MAX_BATCH_SIZE: int = 1000
    PROCESSING_TIMEOUT: int = 30
    
    # Security settings
    SECRET_KEY: str = "your-secret-key"
    ALGORITHM: str = "HS256"
    ACCESS_TOKEN_EXPIRE_MINUTES: int = 30
    
    # Redis settings (для хранения токенов)
    REDIS_HOST: str = "localhost"
    REDIS_PORT: int = 6379
    REDIS_DB: int = 0
    
    # Redis databases
    REDIS_DB_RATE_LIMIT: int = 0
    REDIS_DB_API_KEYS: int = 1
    REDIS_DB_2FA: int = 2
    
    # Rate limiting
    RATE_LIMIT_MAX_REQUESTS: int = 100
    RATE_LIMIT_WINDOW_SECONDS: int = 60
    
    # 2FA
    TWO_FACTOR_ISSUER: str = "SensorAPI"
    
    # Security monitoring
    ELASTICSEARCH_HOST: str = "localhost"
    ELASTICSEARCH_PORT: int = 9200
    
    # Alert settings
    SLACK_WEBHOOK_URL: Optional[str] = None
    TELEGRAM_BOT_TOKEN: Optional[str] = None
    TELEGRAM_CHAT_ID: Optional[str] = None
    
    # Security thresholds
    MAX_LOGIN_ATTEMPTS: int = 5
    SUSPICIOUS_COUNTRIES: List[str] = ["XX", "YY"]  # Список подозрительных стран
    
    # Redis security DB
    REDIS_DB_SECURITY: int = 3
    
    class Config:
        env_file = ".env"

settings = Settings() 