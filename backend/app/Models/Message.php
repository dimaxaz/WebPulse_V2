<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Model;
use Illuminate\Database\Eloquent\Relations\BelongsTo;
use Illuminate\Database\Eloquent\Relations\BelongsToMany;

class Message extends Model
{
    protected $fillable = ['content', 'user_id'];

    protected $with = ['readBy'];

    public function user(): BelongsTo
    {
        return $this->belongsTo(User::class);
    }

    public function readBy(): BelongsToMany
    {
        return $this->belongsToMany(User::class, 'message_reads')
            ->withTimestamps();
    }
} 