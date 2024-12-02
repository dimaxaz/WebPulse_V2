<?php

namespace App\Console\Commands;

use Illuminate\Console\Command;
use Illuminate\Support\Facades\Redis;
use App\Services\MetricsService;

class MonitorSearchPerformance extends Command
{
    protected $signature = 'monitor:search';
    protected $description = 'Monitor search performance metrics';

    private MetricsService $metrics;

    public function __construct(MetricsService $metrics)
    {
        parent::__construct();
        $this->metrics = $metrics;
    }

    public function handle()
    {
        $this->info('Starting performance monitoring...');

        while (true) {
            $cacheHitRate = $this->calculateCacheHitRate();
            $avgSearchTime = $this->calculateAverageSearchTime();
            $activeSearches = $this->getActiveSearchCount();

            $this->table(
                ['Metric', 'Value'],
                [
                    ['Cache Hit Rate', $cacheHitRate . '%'],
                    ['Avg Search Time', round($avgSearchTime, 3) . 's'],
                    ['Active Searches', $activeSearches]
                ]
            );

            sleep(5);
        }
    }

    private function calculateCacheHitRate(): float
    {
        $hits = Redis::get('metrics:cache_hits') ?? 0;
        $total = Redis::get('metrics:total_searches') ?? 1;
        return ($hits / $total) * 100;
    }

    private function calculateAverageSearchTime(): float
    {
        return floatval(Redis::get('metrics:avg_search_time') ?? 0);
    }

    private function getActiveSearchCount(): int
    {
        return Redis::scard('active_searches');
    }
} 