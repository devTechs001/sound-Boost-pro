// src/utils/Profiler.hpp
#pragma once

#include <string>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <iostream>

namespace SoundBoost {

struct ProfileResult {
    std::string name;
    long long durationUs{0};
    int callCount{0};
};

class Profiler {
public:
    static Profiler& getInstance();

    void startSession(const std::string& name);
    void endSession();

    void writeProfile(const std::string& name, long long durationUs);

    void printResults();

private:
    Profiler() = default;
    ~Profiler() = default;

    std::unordered_map<std::string, ProfileResult> m_results;
    std::mutex m_mutex;
    bool m_inSession{false};
    std::string m_sessionName;
};

class ProfileTimer {
public:
    explicit ProfileTimer(const std::string& name)
        : m_name(name)
        , m_start(std::chrono::high_resolution_clock::now())
    {}

    ~ProfileTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - m_start).count();
        Profiler::getInstance().writeProfile(m_name, duration);
    }

private:
    std::string m_name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
};

} // namespace SoundBoost

#define PROFILE_SCOPE(name) SoundBoost::ProfileTimer timer_##__LINE__(name)
