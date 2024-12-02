from fastapi import APIRouter, WebSocket, WebSocketDisconnect, Depends
from typing import Optional
from ..websocket.manager import manager
from ..auth.jwt import get_current_user
from ..models.auth import User

router = APIRouter()

@router.websocket("/ws/sensor/{sensor_id}")
async def websocket_sensor_endpoint(
    websocket: WebSocket,
    sensor_id: int,
    token: Optional[str] = None
):
    try:
        if token:
            # Проверка аутентификации для WebSocket
            user = await get_current_user(
                security_scopes=SecurityScopes(scopes=["sensors:read"]),
                token=token
            )
        
        await manager.connect(websocket, sensor_id)
        
        try:
            while True:
                data = await websocket.receive_text()
                # Обработка входящих сообщений, если необходимо
                await websocket.send_text(f"Message received: {data}")
        except WebSocketDisconnect:
            await manager.disconnect(websocket)
            
    except Exception as e:
        print(f"WebSocket error: {e}")
        if websocket.client_state.connected:
            await websocket.close(code=1000) 