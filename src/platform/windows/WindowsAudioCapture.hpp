// src/platform/windows/WindowsAudioCapture.hpp
#pragma once

#include "../../core/audio/AudioCapture.hpp"

#ifdef PLATFORM_WINDOWS

namespace SoundBoost {

class WindowsAudioCapture : public AudioCapture {
public:
    WindowsAudioCapture() = default;
    ~WindowsAudioCapture() override = default;

    bool initialize(int sampleRate, int bufferSize, int channels) override;
    void start() override;
    void stop() override;

    std::vector<AudioDevice> getAvailableDevices() const override;
    bool setDevice(const std::string& deviceId) override;

private:
    bool m_initialized{false};
    bool m_running{false};
};

// Windows Audio Output
class WindowsAudioOutput : public AudioOutput {
public:
    WindowsAudioOutput() = default;
    ~WindowsAudioOutput() override = default;

    bool initialize(int sampleRate, int bufferSize, int channels) override;
    void start() override;
    void stop() override;

    void write(const AudioBuffer& buffer) override;
    std::vector<AudioDevice> getAvailableDevices() const override;
    bool setDevice(const std::string& deviceId) override;

private:
    bool m_initialized{false};
    bool m_running{false};
};

} // namespace SoundBoost

#endif // PLATFORM_WINDOWS
