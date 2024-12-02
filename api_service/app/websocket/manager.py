from fastapi import WebSocket
from typing import Dict, Set, Optional
import json
import asyncio
from ..models import SensorData

class ConnectionManager:
    def __init__(self):
        self.active_connections: Dict[int, Set[WebSocket]] = {}
        self.sensor_subscriptions: Dict[WebSocket, Set[int]] = {}

    async def connect(self, websocket: WebSocket, sensor_id: Optional[int] = None):
        await websocket.accept()
        if sensor_id:
            if sensor_id not in self.active_connections:
                self.active_connections[sensor_id] = set()
            self.active_connections[sensor_id].add(websocket)
            
            if websocket not in self.sensor_subscriptions:
                self.sensor_subscriptions[websocket] = set()
            self.sensor_subscriptions[websocket].add(sensor_id)

    async def disconnect(self, websocket: WebSocket):
        # Удаляем все подписки для этого соединения
        if websocket in self.sensor_subscriptions:
            for sensor_id in self.sensor_subscriptions[websocket]:
                if sensor_id in self.active_connections:
                    self.active_connections[sensor_id].remove(websocket)
                    if not self.active_connections[sensor_id]:
                        del self.active_connections[sensor_id]
            del self.sensor_subscriptions[websocket]

    async def subscribe(self, websocket: WebSocket, sensor_id: int):
        if sensor_id not in self.active_connections:
            self.active_connections[sensor_id] = set()
        self.active_connections[sensor_id].add(websocket)
        
        if websocket not in self.sensor_subscriptions:
            self.sensor_subscriptions[websocket] = set()
        self.sensor_subscriptions[websocket].add(sensor_id)

    async def unsubscribe(self, websocket: WebSocket, sensor_id: int):
        if sensor_id in self.active_connections:
            self.active_connections[sensor_id].remove(websocket)
            if not self.active_connections[sensor_id]:
                del self.active_connections[sensor_id]
        
        if websocket in self.sensor_subscriptions:
            self.sensor_subscriptions[websocket].remove(sensor_id)

    async def broadcast_sensor_data(self, sensor_data: SensorData):
        if sensor_data.sensor_id in self.active_connections:
            message = sensor_data.model_dump_json()
            for connection in self.active_connections[sensor_data.sensor_id]:
                try:
                    await connection.send_text(message)
                except Exception as e:
                    print(f"Error sending message: {e}")
                    await self.disconnect(connection)

manager = ConnectionManager() 