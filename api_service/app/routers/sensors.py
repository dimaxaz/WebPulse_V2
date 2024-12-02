from fastapi import APIRouter, HTTPException, BackgroundTasks
from typing import List, Optional
from ..models import SensorData, SensorResponse, SensorStats, SensorFilter, BatchSensorData
from ..kafka_client import KafkaClient
from ..config import settings
import asyncio

router = APIRouter()
kafka_client = KafkaClient()

@router.post("/sensor/data", response_model=SensorResponse)
async def send_sensor_data(data: SensorData):
    success = await kafka_client.send_sensor_data(data)
    return SensorResponse(
        success=success,
        message="Data sent successfully" if success else "Failed to send data"
    )

@router.post("/sensor/batch", response_model=SensorResponse)
async def send_batch_data(batch: BatchSensorData):
    if len(batch.items) > settings.MAX_BATCH_SIZE:
        raise HTTPException(
            status_code=400,
            detail=f"Batch size exceeds maximum of {settings.MAX_BATCH_SIZE}"
        )
    
    success = await kafka_client.send_batch(batch.items)
    return SensorResponse(
        success=success,
        message="Batch sent successfully" if success else "Failed to send batch"
    )

@router.get("/sensor/{sensor_id}/stats", response_model=SensorStats)
async def get_sensor_stats(sensor_id: int):
    # Здесь будет логика получения статистики
    pass

@router.post("/sensor/filter", response_model=List[SensorData])
async def filter_sensor_data(filter_params: SensorFilter):
    # Здесь будет логика фильтрации данных
    pass

@router.get("/sensor/health")
async def check_health():
    return {"status": "healthy", "kafka_connected": kafka_client.running} 