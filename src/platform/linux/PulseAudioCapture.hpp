// src/platform/linux/PulseAudioCapture.hpp
#pragma once

#ifdef PLATFORM_LINUX

#include "../../core/audio/AudioCapture.hpp"
#include <pulse/pulseaudio.h>

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace SoundBoost {

class PulseAudioCapture : public AudioCapture {
public:
    PulseAudioCapture();
    ~PulseAudioCapture() override;

    bool initialize(int sampleRate, int bufferSize, int channels) override;
    void start() override;
    void stop() override;

    std::vector<AudioDevice> getAvailableDevices() const override;
    bool setDevice(const std::string& deviceId) override;

    void setCallback(AudioCallback callback) override {
        std::lock_guard<std::mutex> lock(m_callbackMutex);
        m_callback = std::move(callback);
    }

    bool setMonitorMode(bool enabled) { m_monitorMode = enabled; }

private:
    static void contextStateCallback(pa_context* c, void* userdata);
    static void streamReadCallback(pa_stream* s, size_t length, void* userdata);
    static void streamStateCallback(pa_stream* s, void* userdata);

    pa_threaded_mainloop* m_mainloop{nullptr};
    pa_context* m_context{nullptr};
    pa_stream* m_stream{nullptr};

    std::atomic<bool> m_running{false};
    std::atomic<bool> m_contextReady{false};

    AudioCallback m_callback;
    std::mutex m_callbackMutex;

    int m_sampleRate{48000};
    int m_bufferSize{512};
    int m_channels{2};
    bool m_monitorMode{true};

    std::string m_currentDeviceId;
};

} // namespace SoundBoost

#endif // PLATFORM_LINUX
