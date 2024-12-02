#include "SystemMonitor.hpp"
#include <iostream>
#include <chrono>
#include <cstring>

SystemMonitor::SystemMonitor(std::shared_ptr<prometheus::Registry> registry)
    : registry_(registry)
    , cpu_usage_(prometheus::BuildGauge()
        .Name("system_cpu_usage_percent")
        .Help("CPU usage percentage")
        .Register(*registry_))
    , cpu_system_(prometheus::BuildGauge()
        .Name("system_cpu_system_percent")
        .Help("CPU system time percentage")
        .Register(*registry_))
    , cpu_user_(prometheus::BuildGauge()
        .Name("system_cpu_user_percent")
        .Help("CPU user time percentage")
        .Register(*registry_))
    , load_average_1m_(prometheus::BuildGauge()
        .Name("system_load_average_1m")
        .Help("System load average 1m")
        .Register(*registry_))
    , load_average_5m_(prometheus::BuildGauge()
        .Name("system_load_average_5m")
        .Help("System load average 5m")
        .Register(*registry_))
    , memory_used_(prometheus::BuildGauge()
        .Name("system_memory_used_bytes")
        .Help("Used memory in bytes")
        .Register(*registry_))
    , memory_free_(prometheus::BuildGauge()
        .Name("system_memory_free_bytes")
        .Help("Free memory in bytes")
        .Register(*registry_))
    , memory_cached_(prometheus::BuildGauge()
        .Name("system_memory_cached_bytes")
        .Help("Cached memory in bytes")
        .Register(*registry_))
    , memory_swap_used_(prometheus::BuildGauge()
        .Name("system_memory_swap_used_bytes")
        .Help("Used swap in bytes")
        .Register(*registry_))
    , io_read_bytes_(prometheus::BuildGauge()
        .Name("system_io_read_bytes")
        .Help("IO read bytes")
        .Register(*registry_))
    , io_write_bytes_(prometheus::BuildGauge()
        .Name("system_io_write_bytes")
        .Help("IO write bytes")
        .Register(*registry_))
    , io_read_ops_(prometheus::BuildGauge()
        .Name("system_io_read_ops")
        .Help("IO read operations")
        .Register(*registry_))
    , io_write_ops_(prometheus::BuildGauge()
        .Name("system_io_write_ops")
        .Help("IO write operations")
        .Register(*registry_))
    , network_rx_bytes_(prometheus::BuildGauge()
        .Name("system_network_rx_bytes")
        .Help("Network received bytes")
        .Register(*registry_))
    , network_tx_bytes_(prometheus::BuildGauge()
        .Name("system_network_tx_bytes")
        .Help("Network transmitted bytes")
        .Register(*registry_))
    , network_rx_packets_(prometheus::BuildGauge()
        .Name("system_network_rx_packets")
        .Help("Network received packets")
        .Register(*registry_))
    , network_tx_packets_(prometheus::BuildGauge()
        .Name("system_network_tx_packets")
        .Help("Network transmitted packets")
        .Register(*registry_))
{
    cpu_stats_.last_update = std::chrono::steady_clock::now();
}

void SystemMonitor::start() {
    running_ = true;
    monitoring_thread_ = std::thread(&SystemMonitor::monitoringLoop, this);
}

void SystemMonitor::stop() {
    running_ = false;
    if (monitoring_thread_.joinable()) {
        monitoring_thread_.join();
    }
}

void SystemMonitor::monitoringLoop() {
    while (running_) {
        try {
            updateCPUMetrics();
            updateMemoryMetrics();
            updateIOMetrics();
            updateNetworkMetrics();
        } catch (const std::exception& e) {
            std::cerr << "Error in system monitoring: " << e.what() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void SystemMonitor::updateCPUMetrics() {
    std::ifstream stat("/proc/stat");
    std::string line;
    if (std::getline(stat, line) && line.substr(0, 4) == "cpu ") {
        unsigned long long user, nice, system, idle, iowait, irq, softirq;
        std::sscanf(line.c_str(), "cpu %llu %llu %llu %llu %llu %llu %llu",
                    &user, &nice, &system, &idle, &iowait, &irq, &softirq);

        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - cpu_stats_.last_update).count() / 1000.0;

        unsigned long long total_user = user + nice;
        unsigned long long total_system = system + irq + softirq;
        unsigned long long total_idle = idle + iowait;

        if (duration > 0) {
            double user_diff = (total_user - cpu_stats_.last_cpu_user) / duration;
            double system_diff = (total_system - cpu_stats_.last_cpu_system) / duration;
            double idle_diff = (total_idle - cpu_stats_.last_cpu_idle) / duration;
            double total_diff = user_diff + system_diff + idle_diff;

            if (total_diff > 0) {
                cpu_user_.Set((user_diff / total_diff) * 100.0);
                cpu_system_.Set((system_diff / total_diff) * 100.0);
                cpu_usage_.Set(((total_diff - idle_diff) / total_diff) * 100.0);
            }
        }

        cpu_stats_.last_cpu_user = total_user;
        cpu_stats_.last_cpu_system = total_system;
        cpu_stats_.last_cpu_idle = total_idle;
        cpu_stats_.last_update = now;
    }

    double loadavg[3];
    if (getloadavg(loadavg, 3) != -1) {
        load_average_1m_.Set(loadavg[0]);
        load_average_5m_.Set(loadavg[1]);
    }
}

void SystemMonitor::updateMemoryMetrics() {
    struct sysinfo si;
    if (sysinfo(&si) == 0) {
        unsigned long long total_ram = si.totalram * si.mem_unit;
        unsigned long long free_ram = si.freeram * si.mem_unit;
        unsigned long long cached_ram = si.bufferram * si.mem_unit;
        unsigned long long used_ram = total_ram - free_ram - cached_ram;

        memory_used_.Set(used_ram);
        memory_free_.Set(free_ram);
        memory_cached_.Set(cached_ram);
        memory_swap_used_.Set((si.totalswap - si.freeswap) * si.mem_unit);
    }
}

void SystemMonitor::updateIOMetrics() {
    std::ifstream diskstats("/proc/diskstats");
    std::string line;
    unsigned long long reads = 0, writes = 0, read_sectors = 0, write_sectors = 0;

    while (std::getline(diskstats, line)) {
        unsigned long long major, minor, rd, rd_mrg, rd_sec, rd_tim,
                          wr, wr_mrg, wr_sec, wr_tim;
        char dev_name[32];
        if (std::sscanf(line.c_str(),
                       "%llu %llu %s %llu %llu %llu %llu %llu %llu %llu %llu",
                       &major, &minor, dev_name,
                       &rd, &rd_mrg, &rd_sec, &rd_tim,
                       &wr, &wr_mrg, &wr_sec, &wr_tim) == 11) {
            if (major != 7) { // Исключаем loop-устройства
                reads += rd;
                writes += wr;
                read_sectors += rd_sec;
                write_sectors += wr_sec;
            }
        }
    }

    io_read_ops_.Set(reads);
    io_write_ops_.Set(writes);
    io_read_bytes_.Set(read_sectors * 512);  // sector = 512 bytes
    io_write_bytes_.Set(write_sectors * 512);
}

void SystemMonitor::updateNetworkMetrics() {
    std::ifstream netdev("/proc/net/dev");
    std::string line;
    unsigned long long total_rx_bytes = 0, total_tx_bytes = 0,
                      total_rx_packets = 0, total_tx_packets = 0;

    // Пропускаем заголовки
    std::getline(netdev, line);
    std::getline(netdev, line);

    while (std::getline(netdev, line)) {
        std::istringstream iss(line);
        std::string iface;
        unsigned long long rx_bytes, rx_packets, rx_errs, rx_drop,
                          tx_bytes, tx_packets, tx_errs, tx_drop;

        iss >> iface;
        if (iface != "lo:") { // Исключаем loopback
            iss >> rx_bytes >> rx_packets >> rx_errs >> rx_drop
                >> tx_bytes >> tx_packets >> tx_errs >> tx_drop;

            total_rx_bytes += rx_bytes;
            total_tx_bytes += tx_bytes;
            total_rx_packets += rx_packets;
            total_tx_packets += tx_packets;
        }
    }

    network_rx_bytes_.Set(total_rx_bytes);
    network_tx_bytes_.Set(total_tx_bytes);
    network_rx_packets_.Set(total_rx_packets);
    network_tx_packets_.Set(total_tx_packets);
} 