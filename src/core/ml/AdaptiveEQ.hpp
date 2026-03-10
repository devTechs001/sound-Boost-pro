// src/core/ml/AdaptiveEQ.hpp
#pragma once

#include <vector>
#include <string>

namespace SoundBoost {

class AdaptiveEQ {
public:
    AdaptiveEQ() = default;
    ~AdaptiveEQ() = default;

    void initialize(int sampleRate) { m_sampleRate = sampleRate; }

    std::vector<float> suggestSettings(const std::vector<float>& spectralFeatures,
                                       const std::string& genre,
                                       bool hasVoice);

private:
    int m_sampleRate{48000};
};

} // namespace SoundBoost
