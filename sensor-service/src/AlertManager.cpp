#include "AlertManager.hpp"
#include <iostream>

AlertManager::AlertManager(const std::string& webhook_url)
    : webhook_url_(webhook_url) {
    curl_ = curl_easy_init();
    if (!curl_) {
        throw std::runtime_error("Failed to initialize CURL");
    }
}

AlertManager::~AlertManager() {
    if (curl_) {
        curl_easy_cleanup(curl_);
    }
}

void AlertManager::sendAlert(const Alert& alert) {
    auto now = std::chrono::system_clock::now();
    if (now - last_alert_time_ < ALERT_COOLDOWN) {
        return;  // Предотвращаем слишком частые алерты
    }

    nlohmann::json payload = {
        {"text", alert.description},
        {"severity", [&alert]() {
            switch (alert.severity) {
                case AlertSeverity::INFO: return "INFO";
                case AlertSeverity::WARNING: return "WARNING";
                case AlertSeverity::CRITICAL: return "CRITICAL";
            }
            return "UNKNOWN";
        }()},
        {"timestamp", std::chrono::duration_cast<std::chrono::seconds>(
            alert.timestamp.time_since_epoch()).count()}
    };

    sendWebhook(payload);
    last_alert_time_ = now;
}

void AlertManager::checkThresholds(
    double buffer_size,
    double kafka_lag,
    const std::vector<std::pair<int, double>>& sensor_values
) {
    // Проверка размера буфера
    if (buffer_size > 90000) {
        sendAlert({
            "Buffer Nearly Full",
            "Data buffer is approaching maximum capacity",
            AlertSeverity::CRITICAL,
            std::chrono::system_clock::now()
        });
    } else if (buffer_size > 70000) {
        sendAlert({
            "Buffer Warning",
            "Data buffer is filling up",
            AlertSeverity::WARNING,
            std::chrono::system_clock::now()
        });
    }

    // Проверка задержки Kafka
    if (kafka_lag > 1000) {
        sendAlert({
            "High Kafka Lag",
            "Kafka producer is experiencing high lag",
            AlertSeverity::CRITICAL,
            std::chrono::system_clock::now()
        });
    }

    // Проверка значений датчиков
    for (const auto& [sensor_id, value] : sensor_values) {
        if (value < -50 || value > 100) {
            sendAlert({
                "Sensor Value Out of Range",
                "Sensor " + std::to_string(sensor_id) + " reported abnormal value: " + std::to_string(value),
                AlertSeverity::WARNING,
                std::chrono::system_clock::now()
            });
        }
    }
}

size_t AlertManager::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void AlertManager::sendWebhook(const nlohmann::json& payload) {
    std::string response;
    std::string payload_str = payload.dump();

    curl_easy_setopt(curl_, CURLOPT_URL, webhook_url_.c_str());
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, payload_str.c_str());
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl_);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        std::cerr << "Failed to send alert: " << curl_easy_strerror(res) << std::endl;
    }
}