#pragma once

#include <unordered_map>
#include <string>
#include <chrono>
#include <mutex>
#include <vector>

class HotPathAnalyzer {
public:
    struct PathStats {
        std::string path;
        std::chrono::microseconds total_time{0};
        uint64_t calls{0};
        double avg_time{0.0};
    };

    void recordPath(
        const std::string& path,
        std::chrono::microseconds duration
    );
    
    std::vector<PathStats> getHotPaths(size_t top_n = 10) const;
    void reset();
    
    // Макрос для автоматического профилирования функции
    #define PROFILE_FUNCTION() \
        HotPathAnalyzer::ScopedProfile __profile( \
            __FUNCTION__, \
            HotPathAnalyzer::getInstance() \
        )

private:
    class ScopedProfile {
    public:
        ScopedProfile(
            const std::string& path,
            HotPathAnalyzer& analyzer
        );
        ~ScopedProfile();

    private:
        std::string path_;
        HotPathAnalyzer& analyzer_;
        std::chrono::steady_clock::time_point start_;
    };

    mutable std::mutex mutex_;
    std::unordered_map<std::string, PathStats> stats_;

    static HotPathAnalyzer& getInstance();
}; 