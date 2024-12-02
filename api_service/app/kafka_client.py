from aiokafka import AIOKafkaProducer, AIOKafkaConsumer
from typing import List, Any
import json
import asyncio
from datetime import datetime
from .config import settings
from .models import SensorData

class KafkaClient:
    def __init__(self):
        self.producer = None
        self.consumer = None
        self.running = False
        
    async def start(self):
        self.producer = AIOKafkaProducer(
            bootstrap_servers=settings.KAFKA_BOOTSTRAP_SERVERS,
            value_serializer=lambda v: json.dumps(v).encode('utf-8')
        )
        await self.producer.start()
        
        self.consumer = AIOKafkaConsumer(
            settings.KAFKA_SENSOR_TOPIC,
            bootstrap_servers=settings.KAFKA_BOOTSTRAP_SERVERS,
            group_id=settings.KAFKA_CONSUMER_GROUP,
            value_deserializer=lambda m: json.loads(m.decode('utf-8'))
        )
        await self.consumer.start()
        self.running = True
        
    async def stop(self):
        self.running = False
        if self.producer:
            await self.producer.stop()
        if self.consumer:
            await self.consumer.stop()
            
    async def send_sensor_data(self, data: SensorData) -> bool:
        try:
            await self.producer.send_and_wait(
                settings.KAFKA_SENSOR_TOPIC,
                data.model_dump()
            )
            return True
        except Exception as e:
            print(f"Error sending to Kafka: {e}")
            return False
            
    async def send_batch(self, batch: List[SensorData]) -> bool:
        try:
            messages = [data.model_dump() for data in batch]
            await asyncio.gather(*[
                self.producer.send_and_wait(
                    settings.KAFKA_SENSOR_TOPIC,
                    message
                ) for message in messages
            ])
            return True
        except Exception as e:
            print(f"Error sending batch to Kafka: {e}")
            return False
            
    async def consume_messages(self, callback: Any):
        try:
            async for message in self.consumer:
                if not self.running:
                    break
                await callback(message.value)
        except Exception as e:
            print(f"Error consuming messages: {e}") 