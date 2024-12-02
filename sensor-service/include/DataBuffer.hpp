#pragma once

#include <deque>
#include <mutex>
#include <condition_variable>
#include "SensorManager.hpp"

class DataBuffer {
public:
    explicit DataBuffer(size_t max_size = 100000);

    void push(const SensorData& data);
    bool pop(SensorData& data, std::chrono::milliseconds timeout);
    size_t size() const;
    bool empty() const;
    void clear();

private:
    std::deque<SensorData> buffer_;
    const size_t max_size_;
    mutable std::mutex mutex_;
    std::condition_variable not_empty_;
    std::condition_variable not_full_;
}; 