<?php

namespace App\Http\Controllers;

use App\Services\MetricsService;
use Illuminate\Http\Response;

class MetricsController extends Controller
{
    private MetricsService $metrics;

    public function __construct(MetricsService $metrics)
    {
        $this->metrics = $metrics;
    }

    public function show(): Response
    {
        return response($this->metrics->getMetrics())
            ->header('Content-Type', 'text/plain');
    }
} 