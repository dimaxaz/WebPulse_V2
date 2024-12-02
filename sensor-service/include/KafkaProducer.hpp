#pragma once

#include <memory>
#include <string>
#include <atomic>
#include <librdkafka/rdkafkacpp.h>
#include "RetryManager.hpp"

class KafkaProducer {
public:
    struct Stats {
        std::atomic<uint64_t> messages_sent{0};
        std::atomic<uint64_t> messages_failed{0};
        std::atomic<uint64_t> retries{0};
        std::atomic<uint64_t> errors{0};
    };

    KafkaProducer(const std::string& brokers, const std::string& topic);
    ~KafkaProducer();

    bool produce(const std::string& message);
    bool produceWithRetry(const std::string& message);
    void flush(int timeout_ms = 10000);
    Stats getStats() const;

private:
    std::unique_ptr<RdKafka::Producer> producer_;
    std::string topic_;
    std::unique_ptr<RdKafka::Topic> topic_ptr_;
    RetryManager retry_manager_;
    Stats stats_;
}; 