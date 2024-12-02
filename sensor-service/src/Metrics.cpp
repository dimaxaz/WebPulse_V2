#include "Metrics.hpp"

Metrics::Metrics(const std::string& bind_address) 
    : exposer_(std::make_unique<prometheus::Exposer>(bind_address))
    , registry_(std::make_shared<prometheus::Registry>())
    , messages_sent_(prometheus::BuildCounter()
        .Name("sensor_service_messages_sent_total")
        .Help("Total number of messages sent to Kafka")
        .Register(*registry_))
    , messages_failed_(prometheus::BuildCounter()
        .Name("sensor_service_messages_failed_total")
        .Help("Total number of failed message sends")
        .Register(*registry_))
    , retries_(prometheus::BuildCounter()
        .Name("sensor_service_retries_total")
        .Help("Total number of retry attempts")
        .Register(*registry_))
    , errors_(prometheus::BuildCounter()
        .Name("sensor_service_errors_total")
        .Help("Total number of errors")
        .Register(*registry_))
    , sensor_values_(prometheus::BuildGauge()
        .Name("sensor_value")
        .Help("Current sensor values")
        .Labels({{"sensor_id", ""}})
        .Register(*registry_))
    , sensor_status_(prometheus::BuildGauge()
        .Name("sensor_status")
        .Help("Sensor online status (1=online, 0=offline)")
        .Labels({{"sensor_id", ""}})
        .Register(*registry_))
    , processing_time_(prometheus::BuildHistogram()
        .Name("sensor_service_processing_time_seconds")
        .Help("Time spent processing sensor data")
        .Buckets({0.001, 0.005, 0.01, 0.05, 0.1, 0.5, 1.0})
        .Register(*registry_))
    , buffer_size_(prometheus::BuildGauge()
        .Name("sensor_service_buffer_size")
        .Help("Current size of the data buffer")
        .Register(*registry_))
    , kafka_lag_(prometheus::BuildGauge()
        .Name("sensor_service_kafka_lag")
        .Help("Current Kafka producer lag")
        .Register(*registry_))
{
    exposer_->RegisterCollectable(registry_);
}

void Metrics::incrementMessagesSent() {
    messages_sent_.Increment();
}

void Metrics::incrementMessagesFailures() {
    messages_failed_.Increment();
}

void Metrics::incrementRetries() {
    retries_.Increment();
}

void Metrics::incrementErrors() {
    errors_.Increment();
}

void Metrics::recordSensorValue(int sensor_id, double value) {
    sensor_values_.Add({{"sensor_id", std::to_string(sensor_id)}}).Set(value);
}

void Metrics::setSensorStatus(int sensor_id, bool online) {
    sensor_status_.Add({{"sensor_id", std::to_string(sensor_id)}}).Set(online ? 1 : 0);
}

void Metrics::observeProcessingTime(double seconds) {
    processing_time_.Observe(seconds);
}

void Metrics::setBufferSize(double size) {
    buffer_size_.Set(size);
}

void Metrics::setKafkaLag(double lag) {
    kafka_lag_.Set(lag);
}