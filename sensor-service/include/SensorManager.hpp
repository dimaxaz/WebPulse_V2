#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include <thread>
#include <atomic>

struct SensorData {
    int sensor_id;
    double value;
    std::chrono::system_clock::time_point timestamp;
};

class SensorManager {
public:
    using SensorCallback = std::function<void(const SensorData&)>;

    SensorManager(int polling_interval_ms = 100);
    ~SensorManager();

    void addSensor(int sensor_id);
    void start();
    void stop();
    void setCallback(SensorCallback callback);

private:
    void pollingSensorLoop();
    double readSensorValue(int sensor_id);

    std::vector<int> sensor_ids_;
    int polling_interval_ms_;
    SensorCallback callback_;
    std::atomic<bool> running_{false};
    std::thread polling_thread_;
}; 