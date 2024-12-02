<?php

namespace App\Services;

use Elasticsearch\ClientBuilder;
use Elasticsearch\Client;
use App\Models\Message;
use Illuminate\Support\Facades\Log;

class ElasticsearchService
{
    private Client $client;
    private string $index;

    public function __construct()
    {
        $this->client = ClientBuilder::create()
            ->setHosts(config('elasticsearch.hosts'))
            ->build();
        
        $this->index = config('elasticsearch.indices.messages.name');
    }

    public function indexMessage(Message $message): void
    {
        $this->client->index([
            'index' => $this->index,
            'id' => $message->id,
            'body' => [
                'id' => $message->id,
                'content' => $message->content,
                'user_id' => $message->user_id,
                'created_at' => $message->created_at
            ]
        ]);
    }

    public function search(array $params): array
    {
        $startTime = microtime(true);
        
        try {
            $query = [
                'bool' => [
                    'must' => []
                ]
            ];

            if (!empty($params['query'])) {
                $query['bool']['must'][] = [
                    'match' => [
                        'content' => [
                            'query' => $params['query'],
                            'fuzziness' => 'AUTO'
                        ]
                    ]
                ];
            }

            if (!empty($params['user_id'])) {
                $query['bool']['must'][] = [
                    'term' => ['user_id' => $params['user_id']]
                ];
            }

            if (!empty($params['date_from']) || !empty($params['date_to'])) {
                $range = ['created_at' => []];
                
                if (!empty($params['date_from'])) {
                    $range['created_at']['gte'] = $params['date_from'];
                }
                
                if (!empty($params['date_to'])) {
                    $range['created_at']['lte'] = $params['date_to'];
                }
                
                $query['bool']['must'][] = ['range' => $range];
            }

            $response = $this->client->search([
                'index' => $this->index,
                'body' => [
                    'query' => $query,
                    'sort' => [
                        'created_at' => ['order' => 'desc']
                    ],
                    'from' => ($params['page'] - 1) * $params['per_page'],
                    'size' => $params['per_page']
                ]
            ]);

            $duration = microtime(true) - $startTime;
            
            Log::channel('search')->info('Search performed', [
                'query' => $params,
                'hits' => $response['hits']['total']['value'],
                'duration' => $duration,
                'took' => $response['took']
            ]);

            return [
                'total' => $response['hits']['total']['value'],
                'ids' => array_column(array_column($response['hits']['hits'], '_source'), 'id')
            ];
        } catch (\Exception $e) {
            Log::channel('search')->error('Search failed', [
                'query' => $params,
                'error' => $e->getMessage()
            ]);
            
            throw $e;
        }
    }

    public function deleteMessage(int $id): void
    {
        $this->client->delete([
            'index' => $this->index,
            'id' => $id
        ]);
    }
} 