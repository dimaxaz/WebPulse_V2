<?php

namespace App\Providers;

use App\Models\Message;
use App\Observers\MessageObserver;

class AppServiceProvider extends ServiceProvider
{
    public function boot()
    {
        Message::observe(MessageObserver::class);
    }

    public function register()
    {
        //
    }
} 