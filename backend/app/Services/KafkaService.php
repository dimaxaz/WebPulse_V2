<?php

namespace App\Services;

use App\Events\NewMessageEvent;
use Junges\Kafka\Facades\Kafka;
use Junges\Kafka\Message\Message;

class KafkaService
{
    public function produceMessage(string $message): void
    {
        $messageData = [
            'text' => $message,
            'timestamp' => now()->timestamp,
        ];

        // Отправляем в Kafka
        Kafka::publishOn(config('kafka.topics.messages'))
            ->withMessage(new Message(body: $messageData))
            ->send();

        // Отправляем WebSocket событие
        event(new NewMessageEvent($messageData));
    }
} 