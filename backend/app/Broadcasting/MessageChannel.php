<?php

namespace App\Broadcasting;

use App\Models\User;

class MessageChannel
{
    public function join(User $user)
    {
        return true; // Можно добавить дополнительную логику авторизации
    }
} 