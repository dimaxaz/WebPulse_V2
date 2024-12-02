<?php

return [
    'brokers' => env('KAFKA_BROKERS', 'localhost:9092'),
    'topics' => [
        'messages' => env('KAFKA_MESSAGES_TOPIC', 'messages'),
    ],
    'consumer_group' => env('KAFKA_CONSUMER_GROUP', 'webpulse-group'),
]; 