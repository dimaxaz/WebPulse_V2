<?php

namespace App\Http\Controllers;

use App\Models\Message;
use App\Models\User;
use Illuminate\Http\Request;
use Illuminate\Http\JsonResponse;
use Illuminate\Support\Facades\Cache;
use App\Services\ElasticsearchService;
use Illuminate\Support\Facades\Redis;

class MessageController extends Controller
{
    private const CACHE_TTL = 300; // 5 минут
    private ElasticsearchService $elasticsearch;

    public function __construct(ElasticsearchService $elasticsearch)
    {
        $this->elasticsearch = $elasticsearch;
    }

    public function index(Request $request): JsonResponse
    {
        $perPage = $request->input('per_page', 50);
        $messages = Message::with(['user', 'readBy'])
            ->orderBy('created_at', 'desc')
            ->paginate($perPage);

        return response()->json([
            'messages' => $messages->items(),
            'has_more' => $messages->hasMorePages(),
            'total' => $messages->total()
        ]);
    }

    public function store(Request $request): JsonResponse
    {
        $validated = $request->validate([
            'message' => 'required|string|max:255'
        ]);

        try {
            $this->kafkaService->produceMessage($validated['message']);

            return response()->json([
                'status' => 'success',
                'message' => $validated['message'],
                'timestamp' => now()->timestamp
            ]);
        } catch (\Exception $e) {
            return response()->json([
                'status' => 'error',
                'message' => 'Ошибка при отправке сообщения'
            ], 500);
        }
    }

    public function search(Request $request): JsonResponse
    {
        $cacheKey = $this->generateCacheKey($request);
        
        // Пробуем получить из Redis
        $cached = Redis::get($cacheKey);
        if ($cached) {
            return response()->json(json_decode($cached, true));
        }

        // Поиск через Elasticsearch
        $searchResult = $this->elasticsearch->search([
            'query' => $request->input('query'),
            'user_id' => $request->input('user_id'),
            'date_from' => $request->input('date_from'),
            'date_to' => $request->input('date_to'),
            'page' => $request->input('page', 1),
            'per_page' => $request->input('per_page', 50)
        ]);

        // Получаем сообщения из базы данных по ID
        $messages = Message::with(['user', 'readBy'])
            ->whereIn('id', $searchResult['ids'])
            ->get()
            ->sortByDesc('created_at');

        $result = [
            'messages' => $messages->values(),
            'total' => $searchResult['total'],
            'has_more' => $searchResult['total'] > ($request->input('page', 1) * $request->input('per_page', 50))
        ];

        // Кэшируем в Redis
        Redis::setex($cacheKey, 300, json_encode($result));

        return response()->json($result);
    }

    private function generateCacheKey(Request $request): string
    {
        return 'search:' . md5(json_encode([
            'query' => $request->input('query'),
            'user_id' => $request->input('user_id'),
            'date_from' => $request->input('date_from'),
            'date_to' => $request->input('date_to'),
            'page' => $request->input('page', 1),
            'per_page' => $request->input('per_page', 50)
        ]));
    }

    public function users()
    {
        return Cache::remember('users_list', 1800, function () {
            return response()->json(
                User::select('id', 'name')
                    ->orderBy('name')
                    ->get()
            );
        });
    }
} 