#include "SensorService.hpp"
#include <iostream>
#include <csignal>

std::unique_ptr<SensorService> service;

void signalHandler(int signum) {
    std::cout << "Stopping service..." << std::endl;
    if (service) {
        service->stop();
    }
    exit(signum);
}

int main(int argc, char* argv[]) {
    try {
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);

        const std::string kafka_brokers = "localhost:9092";
        const std::string topic = "sensor_data";
        
        service = std::make_unique<SensorService>(kafka_brokers, topic);

        // Добавляем датчики
        for (int i = 1; i <= 5; ++i) {
            service->addSensor(i);
        }

        std::cout << "Starting sensor service..." << std::endl;
        service->start();

        // Держим main поток живым
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
} 