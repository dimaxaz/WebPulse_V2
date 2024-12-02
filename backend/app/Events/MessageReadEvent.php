<?php

namespace App\Events;

use App\Models\User;
use Illuminate\Broadcasting\Channel;
use Illuminate\Broadcasting\InteractsWithSockets;
use Illuminate\Broadcasting\PrivateChannel;
use Illuminate\Contracts\Broadcasting\ShouldBroadcast;
use Illuminate\Foundation\Events\Dispatchable;
use Illuminate\Queue\SerializesModels;

class MessageReadEvent implements ShouldBroadcast
{
    use Dispatchable, InteractsWithSockets, SerializesModels;

    public function __construct(
        public int $messageId,
        public User $reader
    ) {}

    public function broadcastOn(): array
    {
        return [
            new PrivateChannel('messages')
        ];
    }

    public function broadcastWith(): array
    {
        return [
            'messageId' => $this->messageId,
            'reader' => [
                'id' => $this->reader->id,
                'name' => $this->reader->name
            ],
            'readAt' => now()->toISOString()
        ];
    }
} 