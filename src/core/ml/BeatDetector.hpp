// src/core/ml/BeatDetector.hpp
#pragma once

#include <vector>
#include "../audio/AudioBuffer.hpp"

namespace SoundBoost {

class BeatDetector {
public:
    BeatDetector() = default;
    ~BeatDetector() = default;

    bool loadModel(const std::string& modelPath) { return true; }

    float detectBPM(const AudioBuffer& buffer);
    std::vector<float> getBeatPositions() const { return m_beatPositions; }

private:
    std::vector<float> m_beatPositions;
};

} // namespace SoundBoost
