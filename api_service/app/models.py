from pydantic import BaseModel, Field
from typing import Optional, List
from datetime import datetime

class SensorData(BaseModel):
    sensor_id: int = Field(..., gt=0)
    value: float
    timestamp: datetime = Field(default_factory=datetime.utcnow)

class SensorResponse(BaseModel):
    success: bool
    message: str
    data: Optional[dict] = None

class SensorStats(BaseModel):
    sensor_id: int
    avg_value: float
    min_value: float
    max_value: float
    last_update: datetime

class SensorFilter(BaseModel):
    start_time: Optional[datetime] = None
    end_time: Optional[datetime] = None
    min_value: Optional[float] = None
    max_value: Optional[float] = None

class BatchSensorData(BaseModel):
    items: List[SensorData] 