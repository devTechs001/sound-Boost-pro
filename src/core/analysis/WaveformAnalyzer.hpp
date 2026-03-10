// src/core/analysis/WaveformAnalyzer.hpp
#pragma once

#include <vector>
#include "../audio/AudioBuffer.hpp"

namespace SoundBoost {

class WaveformAnalyzer {
public:
    WaveformAnalyzer() = default;
    ~WaveformAnalyzer() = default;

    void initialize(int sampleRate);
    void process(const AudioBuffer& buffer);

    const std::vector<float>& getWaveformData() const { return m_waveformData; }

    float getZeroCrossingRate() const { return m_zcr; }
    float getCrestFactor() const { return m_crestFactor; }

private:
    int m_sampleRate{48000};
    std::vector<float> m_waveformData;
    float m_zcr{0.0f};
    float m_crestFactor{0.0f};
};

} // namespace SoundBoost
