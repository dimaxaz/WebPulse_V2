'default' => env('BROADCAST_DRIVER', 'pusher'),

'connections' => [
    'pusher' => [
        'driver' => 'pusher',
        'key' => env('PUSHER_APP_KEY', 'local'),
        'secret' => env('PUSHER_APP_SECRET', 'local'),
        'app_id' => env('PUSHER_APP_ID', 'local'),
        'options' => [
            'cluster' => env('PUSHER_APP_CLUSTER', 'mt1'),
            'host' => '127.0.0.1',
            'port' => 6001,
            'scheme' => 'http',
            'encrypted' => false,
        ],
    ],
], 