// src/core/ml/VoiceDetector.hpp
#pragma once

#include <vector>
#include <utility>

namespace SoundBoost {

class VoiceDetector {
public:
    VoiceDetector() = default;
    ~VoiceDetector() = default;

    bool loadModel(const std::string& modelPath) { return true; }

    std::pair<bool, float> detect(const std::vector<float>& features) {
        return {false, 0.0f};
    }

private:
    bool m_loaded{false};
};

} // namespace SoundBoost
