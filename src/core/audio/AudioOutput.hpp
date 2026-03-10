// src/core/audio/AudioOutput.hpp
#pragma once

#include "AudioDevice.hpp"
#include "AudioBuffer.hpp"
#include <string>
#include <vector>

namespace SoundBoost {

class AudioOutput {
public:
    virtual ~AudioOutput() = default;

    virtual bool initialize(int sampleRate, int bufferSize, int channels) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;

    virtual void write(const AudioBuffer& buffer) = 0;
    virtual std::vector<AudioDevice> getAvailableDevices() const = 0;
    virtual bool setDevice(const std::string& deviceId) = 0;
};

} // namespace SoundBoost
