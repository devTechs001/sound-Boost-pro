// src/core/analysis/SpectrumAnalyzer.hpp
#pragma once

#include <vector>
#include "../audio/AudioBuffer.hpp"

namespace SoundBoost {

class SpectrumAnalyzer {
public:
    SpectrumAnalyzer() = default;
    ~SpectrumAnalyzer() = default;

    void initialize(int sampleRate, size_t fftSize);
    void process(const AudioBuffer& buffer);

    const std::vector<float>& getMagnitudes() const { return m_magnitudes; }
    const std::vector<float>& getFrequencies() const { return m_frequencies; }

    float getFrequencyBin(int index) const;
    float getMagnitudeAtFrequency(float freq) const;

private:
    void computeFrequencies();

    int m_sampleRate{48000};
    size_t m_fftSize{4096};
    std::vector<float> m_magnitudes;
    std::vector<float> m_frequencies;
};

} // namespace SoundBoost
