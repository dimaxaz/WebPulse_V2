#include "HotPathAnalyzer.hpp"
#include <algorithm>

HotPathAnalyzer& HotPathAnalyzer::getInstance() {
    static HotPathAnalyzer instance;
    return instance;
}

void HotPathAnalyzer::recordPath(
    const std::string& path,
    std::chrono::microseconds duration
) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto& stats = stats_[path];
    stats.path = path;
    stats.total_time += duration;
    stats.calls++;
    stats.avg_time = static_cast<double>(stats.total_time.count()) / stats.calls;
}

std::vector<HotPathAnalyzer::PathStats> HotPathAnalyzer::getHotPaths(
    size_t top_n
) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<PathStats> paths;
    paths.reserve(stats_.size());
    
    for (const auto& [_, stats] : stats_) {
        paths.push_back(stats);
    }
    
    std::sort(paths.begin(), paths.end(),
        [](const PathStats& a, const PathStats& b) {
            return a.avg_time > b.avg_time;
        }
    );
    
    if (paths.size() > top_n) {
        paths.resize(top_n);
    }
    
    return paths;
}

void HotPathAnalyzer::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    stats_.clear();
}

HotPathAnalyzer::ScopedProfile::ScopedProfile(
    const std::string& path,
    HotPathAnalyzer& analyzer
)
    : path_(path)
    , analyzer_(analyzer)
    , start_(std::chrono::steady_clock::now())
{}

HotPathAnalyzer::ScopedProfile::~ScopedProfile() {
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        end - start_
    );
    analyzer_.recordPath(path_, duration);
} 