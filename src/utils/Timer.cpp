// src/utils/Timer.cpp
#include "Timer.hpp"

namespace SoundBoost {

void Timer::start(int intervalMs, Callback callback) {
    m_intervalMs = intervalMs;
    m_callback = std::move(callback);
    m_running = true;

    m_thread = std::thread([this] {
        run();
    });
}

void Timer::stop() {
    m_running = false;
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void Timer::run() {
    while (m_running.load()) {
        auto start = std::chrono::steady_clock::now();

        if (m_callback) {
            m_callback();
        }

        auto end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        int sleepTime = m_intervalMs - static_cast<int>(elapsed);
        if (sleepTime > 0 && m_running.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
        }
    }
}

} // namespace SoundBoost
