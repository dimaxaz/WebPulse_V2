<?php

namespace App\Console\Commands;

use App\Models\Message;
use App\Services\ElasticsearchService;
use Illuminate\Console\Command;

class IndexMessages extends Command
{
    protected $signature = 'messages:index';
    protected $description = 'Index all messages in Elasticsearch';

    private ElasticsearchService $elasticsearch;

    public function __construct(ElasticsearchService $elasticsearch)
    {
        parent::__construct();
        $this->elasticsearch = $elasticsearch;
    }

    public function handle()
    {
        $bar = $this->output->createProgressBar(Message::count());
        
        Message::chunk(100, function ($messages) use ($bar) {
            foreach ($messages as $message) {
                $this->elasticsearch->indexMessage($message);
                $bar->advance();
            }
        });

        $bar->finish();
        $this->info("\nIndexing completed!");
    }
} 