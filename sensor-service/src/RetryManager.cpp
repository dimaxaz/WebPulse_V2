#include "RetryManager.hpp"

RetryManager::RetryManager(
    int max_retries,
    std::chrono::milliseconds initial_delay,
    std::chrono::milliseconds max_delay
)
    : max_retries_(max_retries)
    , initial_delay_(initial_delay)
    , max_delay_(max_delay)
{}

std::chrono::milliseconds RetryManager::calculateDelay(int attempt) {
    // Экспоненциальная задержка с случайным компонентом
    auto base_delay = std::chrono::milliseconds(
        static_cast<long long>(
            initial_delay_.count() * std::pow(2, attempt)
        )
    );

    base_delay = std::min(base_delay, max_delay_);

    // Добавляем случайное отклонение ±25%
    std::uniform_real_distribution<> dist(0.75, 1.25);
    auto jittered_delay = static_cast<long long>(
        base_delay.count() * dist(rng_)
    );

    return std::chrono::milliseconds(jittered_delay);
} 