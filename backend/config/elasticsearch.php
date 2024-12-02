<?php

return [
    'hosts' => [
        env('ELASTICSEARCH_HOST', 'localhost:9200'),
    ],
    'indices' => [
        'messages' => [
            'name' => env('ELASTICSEARCH_INDEX_MESSAGES', 'messages'),
            'settings' => [
                'number_of_shards' => 1,
                'number_of_replicas' => 0,
                'analysis' => [
                    'analyzer' => [
                        'custom_analyzer' => [
                            'type' => 'custom',
                            'tokenizer' => 'standard',
                            'filter' => ['lowercase', 'stop', 'snowball']
                        ]
                    ]
                ]
            ],
            'mappings' => [
                'properties' => [
                    'id' => ['type' => 'integer'],
                    'content' => [
                        'type' => 'text',
                        'analyzer' => 'custom_analyzer'
                    ],
                    'user_id' => ['type' => 'integer'],
                    'created_at' => ['type' => 'date'],
                ]
            ]
        ]
    ]
]; 