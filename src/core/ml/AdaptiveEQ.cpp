// src/core/ml/AdaptiveEQ.cpp
#include "AdaptiveEQ.hpp"

namespace SoundBoost {

std::vector<float> AdaptiveEQ::suggestSettings(const std::vector<float>& spectralFeatures,
                                               const std::string& genre,
                                               bool hasVoice) {
    // Default flat EQ
    std::vector<float> gains(10, 0.0f);

    // Simple genre-based suggestions
    if (genre == "Rock" || genre == "Metal") {
        gains = {4.0f, 3.0f, 2.0f, 0.0f, -1.0f, 0.0f, 2.0f, 3.0f, 4.0f, 4.0f};
    } else if (genre == "Electronic" || genre == "Hip-Hop") {
        gains = {6.0f, 5.0f, 3.0f, 1.0f, 0.0f, 0.0f, 1.0f, 2.0f, 3.0f, 4.0f};
    } else if (genre == "Classical" || genre == "Jazz") {
        gains = {2.0f, 1.0f, 0.0f, 1.0f, 2.0f, 2.0f, 1.0f, 1.0f, 2.0f, 3.0f};
    } else if (genre == "Pop") {
        gains = {3.0f, 2.0f, 1.0f, 0.0f, 0.0f, 1.0f, 2.0f, 3.0f, 3.0f, 2.0f};
    }

    // Boost presence if voice detected
    if (hasVoice) {
        gains[4] += 2.0f; // 500 Hz
        gains[5] += 2.0f; // 1 kHz
        gains[6] += 1.0f; // 2 kHz
    }

    return gains;
}

} // namespace SoundBoost
