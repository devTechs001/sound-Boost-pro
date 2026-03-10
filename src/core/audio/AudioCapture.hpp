// src/core/audio/AudioCapture.hpp
#pragma once

#include "AudioDevice.hpp"
#include "AudioBuffer.hpp"
#include <functional>
#include <string>
#include <vector>

namespace SoundBoost {

class AudioCapture {
public:
    using CaptureCallback = std::function<void(const AudioBuffer&)>;

    virtual ~AudioCapture() = default;

    virtual bool initialize(int sampleRate, int bufferSize, int channels) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;

    virtual void setCallback(CaptureCallback callback) { m_callback = std::move(callback); }
    virtual std::vector<AudioDevice> getAvailableDevices() const = 0;
    virtual bool setDevice(const std::string& deviceId) = 0;

protected:
    CaptureCallback m_callback;
};

} // namespace SoundBoost
