#include "SensorManager.hpp"
#include <random> // Для демонстрации, в реальности здесь будет код работы с реальными датчиками

SensorManager::SensorManager(int polling_interval_ms)
    : polling_interval_ms_(polling_interval_ms) {}

SensorManager::~SensorManager() {
    stop();
}

void SensorManager::addSensor(int sensor_id) {
    sensor_ids_.push_back(sensor_id);
}

void SensorManager::start() {
    if (!running_) {
        running_ = true;
        polling_thread_ = std::thread(&SensorManager::pollingSensorLoop, this);
    }
}

void SensorManager::stop() {
    if (running_) {
        running_ = false;
        if (polling_thread_.joinable()) {
            polling_thread_.join();
        }
    }
}

void SensorManager::setCallback(SensorCallback callback) {
    callback_ = std::move(callback);
}

void SensorManager::pollingSensorLoop() {
    while (running_) {
        for (int sensor_id : sensor_ids_) {
            SensorData data{
                sensor_id,
                readSensorValue(sensor_id),
                std::chrono::system_clock::now()
            };

            if (callback_) {
                callback_(data);
            }
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(polling_interval_ms_)
        );
    }
}

double SensorManager::readSensorValue(int sensor_id) {
    // Демо-реализация, в реальности здесь будет код чтения с реального датчика
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::normal_distribution<> dis(20.0, 5.0);
    
    return dis(gen);
} 