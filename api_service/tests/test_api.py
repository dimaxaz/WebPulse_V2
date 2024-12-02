import pytest
from httpx import AsyncClient
from app.main import app
from app.models import SensorData, BatchSensorData
from datetime import datetime

@pytest.mark.asyncio
async def test_send_sensor_data():
    async with AsyncClient(app=app, base_url="http://test") as client:
        data = SensorData(
            sensor_id=1,
            value=23.5,
            timestamp=datetime.utcnow()
        )
        response = await client.post(
            f"{app.settings.API_V1_PREFIX}/sensor/data",
            json=data.model_dump()
        )
        assert response.status_code == 200
        assert response.json()["success"] == True

@pytest.mark.asyncio
async def test_send_batch_data():
    async with AsyncClient(app=app, base_url="http://test") as client:
        batch = BatchSensorData(items=[
            SensorData(sensor_id=1, value=23.5),
            SensorData(sensor_id=2, value=24.5)
        ])
        response = await client.post(
            f"{app.settings.API_V1_PREFIX}/sensor/batch",
            json=batch.model_dump()
        )
        assert response.status_code == 200
        assert response.json()["success"] == True

@pytest.mark.asyncio
async def test_health_check():
    async with AsyncClient(app=app, base_url="http://test") as client:
        response = await client.get(
            f"{app.settings.API_V1_PREFIX}/sensor/health"
        )
        assert response.status_code == 200
        assert "status" in response.json() 