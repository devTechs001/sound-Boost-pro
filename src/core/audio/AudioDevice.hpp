// src/core/audio/AudioDevice.hpp
#pragma once

#include <string>
#include <vector>

namespace SoundBoost {

enum class DeviceType {
    Input,
    Output,
    FullDuplex
};

struct AudioDevice {
    std::string id;
    std::string name;
    DeviceType type;
    int maxChannels{2};
    int sampleRate{48000};
    bool isDefault{false};
};

} // namespace SoundBoost
