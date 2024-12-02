<?php

namespace App\Http\Middleware;

use App\Services\MetricsService;
use Closure;
use Illuminate\Http\Request;

class TrackMetrics
{
    private MetricsService $metrics;

    public function __construct(MetricsService $metrics)
    {
        $this->metrics = $metrics;
    }

    public function handle(Request $request, Closure $next)
    {
        $startTime = microtime(true);
        
        $response = $next($request);
        
        $duration = microtime(true) - $startTime;
        
        if ($request->is('api/messages/search')) {
            $this->metrics->trackSearchRequest($response->status());
            $this->metrics->trackSearchDuration($duration, 'api');
        }

        return $response;
    }
} 