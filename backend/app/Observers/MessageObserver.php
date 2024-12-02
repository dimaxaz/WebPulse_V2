<?php

namespace App\Observers;

use App\Models\Message;
use Illuminate\Support\Facades\Redis;
use App\Services\ElasticsearchService;

class MessageObserver
{
    private ElasticsearchService $elasticsearch;

    public function __construct(ElasticsearchService $elasticsearch)
    {
        $this->elasticsearch = $elasticsearch;
    }

    public function created(Message $message)
    {
        $this->elasticsearch->indexMessage($message);
        $this->clearCache();
    }

    public function updated(Message $message)
    {
        $this->elasticsearch->indexMessage($message);
        $this->clearCache();
    }

    public function deleted(Message $message)
    {
        $this->elasticsearch->deleteMessage($message->id);
        $this->clearCache();
    }

    private function clearCache()
    {
        Redis::del(Redis::keys('search:*'));
    }
} 