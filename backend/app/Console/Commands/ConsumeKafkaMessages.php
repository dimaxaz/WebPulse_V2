<?php

namespace App\Console\Commands;

use Illuminate\Console\Command;
use Junges\Kafka\Facades\Kafka;

class ConsumeKafkaMessages extends Command
{
    protected $signature = 'kafka:consume';
    protected $description = 'Consume messages from Kafka';

    public function handle()
    {
        $topic = config('kafka.topics.messages');
        $consumer = Kafka::createConsumer([$topic])
            ->withConsumerGroupId(config('kafka.consumer_group'))
            ->withAutoCommit()
            ->build();

        $consumer->consume(function($message) {
            $this->info("Получено сообщение: " . json_encode($message->getBody()));
        });
    }
} 