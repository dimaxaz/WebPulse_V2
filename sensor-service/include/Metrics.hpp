#pragma once

#include <prometheus/counter.h>
#include <prometheus/gauge.h>
#include <prometheus/histogram.h>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>
#include <memory>
#include <string>

class Metrics {
public:
    explicit Metrics(const std::string& bind_address = "0.0.0.0:8080");
    
    // Счетчики
    void incrementMessagesSent();
    void incrementMessagesFailures();
    void incrementRetries();
    void incrementErrors();
    
    // Метрики датчиков
    void recordSensorValue(int sensor_id, double value);
    void setSensorStatus(int sensor_id, bool online);
    
    // Метрики производительности
    void observeProcessingTime(double seconds);
    void setBufferSize(double size);
    void setKafkaLag(double lag);

private:
    std::unique_ptr<prometheus::Exposer> exposer_;
    std::shared_ptr<prometheus::Registry> registry_;
    
    // Счетчики
    prometheus::Counter& messages_sent_;
    prometheus::Counter& messages_failed_;
    prometheus::Counter& retries_;
    prometheus::Counter& errors_;
    
    // Датчики
    prometheus::Family<prometheus::Gauge>& sensor_values_;
    prometheus::Family<prometheus::Gauge>& sensor_status_;
    
    // Производительность
    prometheus::Histogram& processing_time_;
    prometheus::Gauge& buffer_size_;
    prometheus::Gauge& kafka_lag_;
}; 