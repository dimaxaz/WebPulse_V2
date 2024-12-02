<?php

namespace App\Http\Controllers;

use App\Events\MessageReadEvent;
use App\Models\Message;
use Illuminate\Http\Request;

class MessageReadController extends Controller
{
    public function markAsRead(Request $request, Message $message)
    {
        $user = $request->user();
        
        if (!$message->readBy->contains($user->id)) {
            $message->readBy()->attach($user->id);
            
            broadcast(new MessageReadEvent(
                $message->id,
                $user
            ))->toOthers();
        }

        return response()->json(['status' => 'success']);
    }
} 