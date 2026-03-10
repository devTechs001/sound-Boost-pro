// src/platform/macos/MacOSAudioCapture.hpp
#pragma once

#include "../../core/audio/AudioCapture.hpp"

#ifdef PLATFORM_MACOS

namespace SoundBoost {

class MacOSAudioCapture : public AudioCapture {
public:
    MacOSAudioCapture() = default;
    ~MacOSAudioCapture() override = default;

    bool initialize(int sampleRate, int bufferSize, int channels) override;
    void start() override;
    void stop() override;

    std::vector<AudioDevice> getAvailableDevices() const override;
    bool setDevice(const std::string& deviceId) override;

private:
    bool m_initialized{false};
    bool m_running{false};
};

// macOS Audio Output
class MacOSAudioOutput : public AudioOutput {
public:
    MacOSAudioOutput() = default;
    ~MacOSAudioOutput() override = default;

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

#endif // PLATFORM_MACOS
