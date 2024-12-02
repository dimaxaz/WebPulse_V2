#include "DataBuffer.hpp"

DataBuffer::DataBuffer(size_t max_size)
    : max_size_(max_size) {}

void DataBuffer::push(const SensorData& data) {
    std::unique_lock<std::mutex> lock(mutex_);
    
    not_full_.wait(lock, [this]() {
        return buffer_.size() < max_size_;
    });

    buffer_.push_back(data);
    lock.unlock();
    not_empty_.notify_one();
}

bool DataBuffer::pop(SensorData& data, std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(mutex_);
    
    if (!not_empty_.wait_for(lock, timeout, [this]() {
        return !buffer_.empty();
    })) {
        return false;
    }

    data = buffer_.front();
    buffer_.pop_front();
    lock.unlock();
    not_full_.notify_one();
    return true;
}

size_t DataBuffer::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.size();
}

bool DataBuffer::empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.empty();
}

void DataBuffer::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    buffer_.clear();
    not_full_.notify_all();
} 