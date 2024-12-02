<?php

use App\Http\Controllers\AuthController;
use App\Http\Controllers\MessageController;
use App\Http\Controllers\TypingController;
use App\Http\Controllers\MessageReadController;
use Illuminate\Support\Facades\Route;

// Публичные маршруты
Route::post('/register', [AuthController::class, 'register']);
Route::post('/login', [AuthController::class, 'login']);

// Защищенные маршруты
Route::middleware('auth:sanctum')->group(function () {
    Route::post('/logout', [AuthController::class, 'logout']);
    Route::post('/messages', [MessageController::class, 'store']);
    Route::post('/typing', [TypingController::class, 'update']);
    Route::post('/messages/{message}/read', [MessageReadController::class, 'markAsRead']);
    Route::get('/messages/search', [MessageController::class, 'search']);
    Route::get('/users', [MessageController::class, 'users']);
}); 