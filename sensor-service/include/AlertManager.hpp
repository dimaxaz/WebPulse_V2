#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

enum class AlertSeverity {
    INFO,
    WARNING,
    CRITICAL
};

struct Alert {
    std::string name;
    std::string description;
    AlertSeverity severity;
    std::chrono::system_clock::time_point timestamp;
};

class AlertManager {
public:
    explicit AlertManager(const std::string& webhook_url);
    ~AlertManager();

    void sendAlert(const Alert& alert);
    void checkThresholds(double buffer_size, double kafka_lag, const std::vector<std::pair<int, double>>& sensor_values);

private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    void sendWebhook(const nlohmann::json& payload);

    std::string webhook_url_;
    CURL* curl_;
    std::chrono::system_clock::time_point last_alert_time_;
    static constexpr auto ALERT_COOLDOWN = std::chrono::minutes(5);
}; 