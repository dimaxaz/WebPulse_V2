<?php

use Illuminate\Support\Facades\Broadcast;

Broadcast::channel('messages.{userId}', function ($user, $userId) {
    return (int) $user->id === (int) $userId;
});

Broadcast::channel('online', function ($user) {
    if ($user) {
        return [
            'id' => $user->id,
            'name' => $user->name
        ];
    }
}); 