#pragma once

#include <gperftools/profiler.h>
#include <gperftools/heap-profiler.h>
#include <string>
#include <chrono>
#include <memory>
#include <atomic>
#include <thread>

class Profiler {
public:
    enum class ProfileType {
        CPU,
        HEAP,
        GROWTH
    };

    explicit Profiler(const std::string& output_dir);
    ~Profiler();

    void startProfiling(ProfileType type);
    void stopProfiling(ProfileType type);
    void startContinuousProfiling(
        ProfileType type,
        std::chrono::seconds interval = std::chrono::seconds(60)
    );
    void stopContinuousProfiling();
    void generateFlameGraph(const std::string& profile_path);

private:
    void continuousProfilingLoop();
    std::string getProfilePath(ProfileType type);
    void setupProfilerOptions();

    std::string output_dir_;
    std::atomic<bool> continuous_running_{false};
    std::thread continuous_thread_;
    ProfileType continuous_type_;
    std::chrono::seconds continuous_interval_;

    static constexpr int kProfilerFrequency = 1000;  // 1000Hz sampling
    static constexpr int kHeapSamplingInterval = 524288;  // 512KB
}; 