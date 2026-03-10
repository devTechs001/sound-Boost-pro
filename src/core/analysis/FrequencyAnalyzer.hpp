// src/core/analysis/FrequencyAnalyzer.hpp
#pragma once

#include <vector>
#include "../audio/AudioBuffer.hpp"

namespace SoundBoost {

class FrequencyAnalyzer {
public:
    FrequencyAnalyzer() = default;
    ~FrequencyAnalyzer() = default;

    void initialize(int sampleRate, size_t fftSize);
    void process(const AudioBuffer& buffer);

    float detectFundamentalFrequency() const;
    float getSpectralCentroid() const { return m_spectralCentroid; }
    float getSpectralTilt() const { return m_spectralTilt; }

private:
    int m_sampleRate{48000};
    size_t m_fftSize{4096};
    std::vector<float> m_magnitudes;
    float m_spectralCentroid{0.0f};
    float m_spectralTilt{0.0f};
};

} // namespace SoundBoost
