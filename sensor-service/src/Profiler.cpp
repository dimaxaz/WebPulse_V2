#include "Profiler.hpp"
#include <sstream>
#include <ctime>
#include <filesystem>
#include <iostream>

Profiler::Profiler(const std::string& output_dir)
    : output_dir_(output_dir) {
    std::filesystem::create_directories(output_dir);
    setupProfilerOptions();
}

Profiler::~Profiler() {
    stopContinuousProfiling();
}

void Profiler::setupProfilerOptions() {
    // CPU профайлер
    ProfilerSetOptions(ProfilerOptions()
        .set_frequency(kProfilerFrequency)
        .set_max_stack_depth(50));

    // Heap профайлер
    HeapProfilerSetOptions(HeapProfilerOptions()
        .set_mmap_log(true)
        .set_allocation_interval(kHeapSamplingInterval));
}

std::string Profiler::getProfilePath(ProfileType type) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << output_dir_ << "/";
    
    switch (type) {
        case ProfileType::CPU:
            ss << "cpu_profile_";
            break;
        case ProfileType::HEAP:
            ss << "heap_profile_";
            break;
        case ProfileType::GROWTH:
            ss << "growth_profile_";
            break;
    }
    
    ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    return ss.str();
}

void Profiler::startProfiling(ProfileType type) {
    std::string profile_path = getProfilePath(type);
    
    switch (type) {
        case ProfileType::CPU:
            ProfilerStart(profile_path.c_str());
            break;
        case ProfileType::HEAP:
            HeapProfilerStart(profile_path.c_str());
            break;
        case ProfileType::GROWTH:
            HeapProfilerStart(profile_path.c_str());
            break;
    }
}

void Profiler::stopProfiling(ProfileType type) {
    switch (type) {
        case ProfileType::CPU:
            ProfilerStop();
            break;
        case ProfileType::HEAP:
        case ProfileType::GROWTH:
            HeapProfilerStop();
            break;
    }
}

void Profiler::startContinuousProfiling(
    ProfileType type,
    std::chrono::seconds interval
) {
    if (!continuous_running_) {
        continuous_type_ = type;
        continuous_interval_ = interval;
        continuous_running_ = true;
        continuous_thread_ = std::thread(
            &Profiler::continuousProfilingLoop,
            this
        );
    }
}

void Profiler::stopContinuousProfiling() {
    if (continuous_running_) {
        continuous_running_ = false;
        if (continuous_thread_.joinable()) {
            continuous_thread_.join();
        }
    }
}

void Profiler::continuousProfilingLoop() {
    while (continuous_running_) {
        std::string profile_path = getProfilePath(continuous_type_);
        startProfiling(continuous_type_);
        
        std::this_thread::sleep_for(continuous_interval_);
        
        stopProfiling(continuous_type_);
        generateFlameGraph(profile_path);
    }
}

void Profiler::generateFlameGraph(const std::string& profile_path) {
    // Генерация FlameGraph с использованием perf
    std::string cmd;
    
    if (std::filesystem::exists(profile_path)) {
        switch (continuous_type_) {
            case ProfileType::CPU:
                cmd = "pprof --collapsed " + profile_path + " > " +
                      profile_path + ".collapsed && " +
                      "flamegraph.pl " + profile_path + ".collapsed > " +
                      profile_path + ".svg";
                break;
                
            case ProfileType::HEAP:
            case ProfileType::GROWTH:
                cmd = "pprof --collapsed --inuse_space " + profile_path + " > " +
                      profile_path + ".collapsed && " +
                      "flamegraph.pl " + profile_path + ".collapsed > " +
                      profile_path + ".svg";
                break;
        }
        
        if (system(cmd.c_str()) != 0) {
            std::cerr << "Failed to generate FlameGraph for " 
                      << profile_path << std::endl;
        }
    }
} 