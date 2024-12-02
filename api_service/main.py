from fastapi import FastAPI

app = FastAPI()

@app.post("/messages")
async def create_message(message: dict):
    # Логика обработки сообщений
    return {"status": "success"} 