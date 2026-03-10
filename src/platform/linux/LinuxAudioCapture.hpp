// src/platform/linux/LinuxAudioCapture.hpp
#pragma once

#include "../../core/audio/AudioCapture.hpp"

#ifdef PLATFORM_LINUX

namespace SoundBoost {

class LinuxAudioCapture : public AudioCapture {
public:
    LinuxAudioCapture() = default;
    ~LinuxAudioCapture() override = default;

    bool initialize(int sampleRate, int bufferSize, int channels) override;
    void start() override;
    void stop() override;

    std::vector<AudioDevice> getAvailableDevices() const override;
    bool setDevice(const std::string& deviceId) override;

private:
    bool m_initialized{false};
    bool m_running{false};
};

// Linux Audio Output
class LinuxAudioOutput : public AudioOutput {
public:
    LinuxAudioOutput() = default;
    ~LinuxAudioOutput() override = default;

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

#endif // PLATFORM_LINUX
