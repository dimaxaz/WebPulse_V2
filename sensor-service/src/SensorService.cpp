#include "SensorService.hpp"
#include <iostream>
#include <iomanip>
#include "Metrics.hpp"
#include "AlertManager.hpp"
#include "SystemMonitor.hpp"
#include "Tracer.hpp"
#include "Profiler.hpp"
#include "HotPathAnalyzer.hpp"

SensorService::SensorService(
    const std::string& kafka_brokers,
    const std::string& topic,
    int polling_interval_ms
) {
    producer_ = std::make_unique<KafkaProducer>(kafka_brokers, topic);
    sensor_manager_ = std::make_unique<SensorManager>(polling_interval_ms);
    buffer_ = std::make_unique<DataBuffer>();
    
    sensor_manager_->setCallback(
        [this](const SensorData& data) {
            handleSensorData(data);
        }
    );
    
    metrics_ = std::make_unique<Metrics>();
    alert_manager_ = std::make_unique<AlertManager>("http://localhost:8080/alert");
    system_monitor_ = std::make_unique<SystemMonitor>(metrics_->getRegistry());
    tracer_ = std::make_unique<Tracer>("sensor_service");
    profiler_ = std::make_unique<Profiler>("profiles");
}

SensorService::~SensorService() {
    stop();
}

void SensorService::start() {
    PROFILE_FUNCTION();
    auto span = tracer_->startSpan("service_start");
    
    try {
        running_ = true;
        system_monitor_->start();
        processing_thread_ = std::thread(&SensorService::processingLoop, this);
        monitoring_thread_ = std::thread(&SensorService::monitoringLoop, this);
        sensor_manager_->start();
        
        profiler_->startContinuousProfiling(
            Profiler::ProfileType::CPU,
            std::chrono::minutes(5)
        );
        
        profiler_->startProfiling(Profiler::ProfileType::HEAP);
        
        tracer_->addEvent(span, "service_started");
    } catch (const std::exception& e) {
        tracer_->setError(span, e.what());
        throw;
    }
    
    span->End();
}

void SensorService::stop() {
    PROFILE_FUNCTION();
    profiler_->stopContinuousProfiling();
    profiler_->stopProfiling(Profiler::ProfileType::HEAP);
    
    auto hot_paths = HotPathAnalyzer::getInstance().getHotPaths();
    for (const auto& path : hot_paths) {
        std::cout << "Hot path: " << path.path
                  << ", avg time: " << path.avg_time
                  << "µs, calls: " << path.calls << std::endl;
    }
    
    running_ = false;
    sensor_manager_->stop();
    
    if (processing_thread_.joinable()) {
        processing_thread_.join();
    }
    if (monitoring_thread_.joinable()) {
        monitoring_thread_.join();
    }
    
    producer_->flush();
}

void SensorService::handleSensorData(const SensorData& data) {
    PROFILE_FUNCTION();
    auto span = tracer_->startSpan("handle_sensor_data", {
        {"sensor_id", std::to_string(data.sensor_id)},
        {"value", std::to_string(data.value)}
    });

    try {
        buffer_->push(data);
        tracer_->addEvent(span, "data_buffered");
    } catch (const std::exception& e) {
        tracer_->setError(span, e.what());
        std::cerr << "Error buffering sensor data: " << e.what() << std::endl;
    }

    span->End();
}

void SensorService::processingLoop() {
    while (running_) {
        SensorData data;
        if (buffer_->pop(data, std::chrono::milliseconds(100))) {
            try {
                std::string message = serializeSensorData(data);
                producer_->produceWithRetry(message);
            } catch (const std::exception& e) {
                std::cerr << "Error processing sensor data: " << e.what() << std::endl;
            }
        }
    }
}

void SensorService::monitoringLoop() {
    while (running_) {
        auto stats = producer_->getStats();
        auto buffer_size = buffer_->size();
        
        // Обновляем метрики
        metrics_->setBufferSize(buffer_size);
        metrics_->setKafkaLag(stats.messages_failed);
        
        // Собираем значения датчиков
        std::vector<std::pair<int, double>> sensor_values;
        for (const auto& sensor : sensor_manager_->getSensors()) {
            double value = sensor.getCurrentValue();
            sensor_values.emplace_back(sensor.id, value);
            metrics_->recordSensorValue(sensor.id, value);
            metrics_->setSensorStatus(sensor.id, sensor.isOnline());
        }
        
        // Проверяем пороговые значения
        alert_manager_->checkThresholds(buffer_size, stats.messages_failed, sensor_values);
        
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

std::string SensorService::serializeSensorData(const SensorData& data) {
    nlohmann::json j;
    j["sensor_id"] = data.sensor_id;
    j["value"] = data.value;
    j["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
        data.timestamp.time_since_epoch()
    ).count();
    
    return j.dump();
} 