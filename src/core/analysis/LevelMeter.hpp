// src/core/analysis/LevelMeter.hpp
#pragma once

#include <vector>
#include "../audio/AudioBuffer.hpp"

namespace SoundBoost {

class LevelMeter {
public:
    LevelMeter() = default;
    ~LevelMeter() = default;

    void initialize(int sampleRate, int channels = 2);
    void process(const AudioBuffer& buffer);

    float getPeakLevel(int channel = 0) const;
    float getRMSLevel(int channel = 0) const;
    float getDBFS(int channel = 0) const;

    void reset();

private:
    int m_sampleRate{48000};
    int m_channels{2};
    std::vector<float> m_peakLevels;
    std::vector<float> m_rmsLevels;
    float m_decayRate{0.001f};
};

} // namespace SoundBoost
