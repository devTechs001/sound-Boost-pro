// src/utils/Timer.hpp
#pragma once

#include <chrono>
#include <functional>
#include <thread>
#include <atomic>

namespace SoundBoost {

class Timer {
public:
    using Callback = std::function<void()>;

    Timer() = default;
    ~Timer() { stop(); }

    void start(int intervalMs, Callback callback);
    void stop();
    void setCallback(Callback callback) { m_callback = std::move(callback); }

    bool isRunning() const { return m_running.load(); }

private:
    void run();

    Callback m_callback;
    std::thread m_thread;
    std::atomic<bool> m_running{false};
    int m_intervalMs{0};
};

// High-resolution timer for measurements
class HighResolutionTimer {
public:
    HighResolutionTimer() { reset(); }

    void reset() {
        m_start = std::chrono::high_resolution_clock::now();
    }

    double elapsedMs() const {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(now - m_start).count();
    }

    double elapsedUs() const {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::micro>(now - m_start).count();
    }

    double elapsedSeconds() const {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(now - m_start).count();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
};

} // namespace SoundBoost
