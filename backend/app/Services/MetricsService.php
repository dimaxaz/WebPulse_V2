<?php

namespace App\Services;

use Prometheus\CollectorRegistry;
use Prometheus\Storage\Redis;

class MetricsService
{
    private CollectorRegistry $registry;
    private array $counters = [];
    private array $histograms = [];

    public function __construct()
    {
        Redis::setDefaultOptions([
            'host' => env('REDIS_HOST', 'localhost'),
            'port' => env('REDIS_PORT', 6379),
            'password' => env('REDIS_PASSWORD', null),
            'timeout' => 0.1,
        ]);
        
        $this->registry = new CollectorRegistry(new Redis());
        $this->initializeMetrics();
    }

    private function initializeMetrics(): void
    {
        // Счетчики
        $this->counters['search_requests'] = $this->registry->getOrRegisterCounter(
            'app',
            'search_requests_total',
            'Total number of search requests',
            ['status']
        );

        $this->counters['cache_hits'] = $this->registry->getOrRegisterCounter(
            'app',
            'cache_hits_total',
            'Total number of cache hits'
        );

        // Гистограммы для времени выполнения
        $this->histograms['search_duration'] = $this->registry->getOrRegisterHistogram(
            'app',
            'search_duration_seconds',
            'Search request duration in seconds',
            ['type'],
            [0.01, 0.05, 0.1, 0.5, 1, 2, 5]
        );
    }

    public function trackSearchRequest(string $status): void
    {
        $this->counters['search_requests']->inc(['status' => $status]);
    }

    public function trackCacheHit(): void
    {
        $this->counters['cache_hits']->inc();
    }

    public function trackSearchDuration(float $duration, string $type): void
    {
        $this->histograms['search_duration']->observe($duration, ['type' => $type]);
    }

    public function getMetrics(): string
    {
        return $this->registry->getMetricFamilySamples();
    }
} 