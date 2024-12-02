#pragma once

#include <prometheus/gauge.h>
#include <prometheus/registry.h>
#include <memory>
#include <thread>
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <fstream>

class SystemMonitor {
public:
    explicit SystemMonitor(std::shared_ptr<prometheus::Registry> registry);
    ~SystemMonitor();

    void start();
    void stop();

private:
    void monitoringLoop();
    void updateCPUMetrics();
    void updateMemoryMetrics();
    void updateIOMetrics();
    void updateNetworkMetrics();
    double getCPUUsage();
    
    std::shared_ptr<prometheus::Registry> registry_;
    std::atomic<bool> running_{false};
    std::thread monitoring_thread_;

    // CPU метрики
    prometheus::Gauge& cpu_usage_;
    prometheus::Gauge& cpu_system_;
    prometheus::Gauge& cpu_user_;
    prometheus::Gauge& load_average_1m_;
    prometheus::Gauge& load_average_5m_;

    // Память
    prometheus::Gauge& memory_used_;
    prometheus::Gauge& memory_free_;
    prometheus::Gauge& memory_cached_;
    prometheus::Gauge& memory_swap_used_;

    // IO
    prometheus::Gauge& io_read_bytes_;
    prometheus::Gauge& io_write_bytes_;
    prometheus::Gauge& io_read_ops_;
    prometheus::Gauge& io_write_ops_;

    // Сеть
    prometheus::Gauge& network_rx_bytes_;
    prometheus::Gauge& network_tx_bytes_;
    prometheus::Gauge& network_rx_packets_;
    prometheus::Gauge& network_tx_packets_;

    // Предыдущие значения для расчета дельты
    struct {
        unsigned long long last_cpu_user{0};
        unsigned long long last_cpu_system{0};
        unsigned long long last_cpu_idle{0};
        std::chrono::steady_clock::time_point last_update;
    } cpu_stats_;

    struct {
        unsigned long long last_rx_bytes{0};
        unsigned long long last_tx_bytes{0};
        unsigned long long last_rx_packets{0};
        unsigned long long last_tx_packets{0};
    } network_stats_;
}; 