<?php

namespace App\Http\Controllers;

use App\Events\UserTypingEvent;
use Illuminate\Http\Request;

class TypingController extends Controller
{
    public function update(Request $request)
    {
        $validated = $request->validate([
            'isTyping' => 'required|boolean'
        ]);

        broadcast(new UserTypingEvent(
            $request->user(),
            $validated['isTyping']
        ))->toOthers();

        return response()->json(['status' => 'success']);
    }
} 