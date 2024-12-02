#pragma once

#include <chrono>
#include <random>
#include <functional>

class RetryManager {
public:
    RetryManager(
        int max_retries = 3,
        std::chrono::milliseconds initial_delay = std::chrono::milliseconds(100),
        std::chrono::milliseconds max_delay = std::chrono::milliseconds(5000)
    );

    template<typename Func>
    bool executeWithRetry(Func&& func);

private:
    std::chrono::milliseconds calculateDelay(int attempt);
    
    const int max_retries_;
    const std::chrono::milliseconds initial_delay_;
    const std::chrono::milliseconds max_delay_;
    std::mt19937 rng_{std::random_device{}()};
};

template<typename Func>
bool RetryManager::executeWithRetry(Func&& func) {
    for (int attempt = 0; attempt < max_retries_; ++attempt) {
        try {
            if (func()) {
                return true;
            }
        } catch (const std::exception& e) {
            if (attempt == max_retries_ - 1) {
                throw;
            }
        }

        auto delay = calculateDelay(attempt);
        std::this_thread::sleep_for(delay);
    }
    return false;
} 