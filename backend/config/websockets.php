<?php

return [
    'dashboard' => [
        'port' => 6001,
    ],

    'apps' => [
        [
            'id' => env('PUSHER_APP_ID'),
            'name' => env('APP_NAME'),
            'key' => env('PUSHER_APP_KEY'),
            'secret' => env('PUSHER_APP_SECRET'),
            'enable_client_messages' => false,
            'enable_statistics' => true,
            'host' => '127.0.0.1',
            'port' => 6001,
            'scheme' => 'http'
        ],
    ],
]; 