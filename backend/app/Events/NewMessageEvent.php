<?php

namespace App\Events;

use Illuminate\Broadcasting\Channel;
use Illuminate\Broadcasting\InteractsWithSockets;
use Illuminate\Broadcasting\PrivateChannel;
use Illuminate\Contracts\Broadcasting\ShouldBroadcast;
use Illuminate\Foundation\Events\Dispatchable;
use Illuminate\Queue\SerializesModels;

class NewMessageEvent implements ShouldBroadcast
{
    use Dispatchable, InteractsWithSockets, SerializesModels;

    public function __construct(
        public array $message,
        public int $userId
    ) {}

    public function broadcastOn(): array
    {
        return [
            new PrivateChannel('messages.' . $this->userId)
        ];
    }

    public function broadcastWith(): array
    {
        return [
            'message' => $this->message,
            'timestamp' => now()->timestamp
        ];
    }
} 