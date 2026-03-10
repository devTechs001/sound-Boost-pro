// src/core/dsp/FilterDesign.hpp
#pragma once

#include "BiquadFilter.hpp"
#include <cmath>
#include <numbers>
#include <vector>
#include <utility>

namespace SoundBoost {

class FilterDesign {
public:
    // Standard biquad filter types
    static BiquadCoefficients lowPass(float sampleRate, float cutoff, float Q);
    static BiquadCoefficients highPass(float sampleRate, float cutoff, float Q);
    static BiquadCoefficients bandPass(float sampleRate, float center, float bandwidth);
    static BiquadCoefficients notch(float sampleRate, float center, float Q);
    static BiquadCoefficients allPass(float sampleRate, float center, float Q);

    // Parametric EQ
    static BiquadCoefficients peakEQ(float sampleRate, float center, float gainDb, float Q);
    static BiquadCoefficients lowShelf(float sampleRate, float cutoff, float gainDb, float slope);
    static BiquadCoefficients highShelf(float sampleRate, float cutoff, float gainDb, float slope);

    // Butterworth filter design (cascaded sections)
    static std::vector<BiquadCoefficients> butterworthLowPass(
        float sampleRate, float cutoff, int order);
    static std::vector<BiquadCoefficients> butterworthHighPass(
        float sampleRate, float cutoff, int order);

    // Linkwitz-Riley crossover filters
    static std::pair<std::vector<BiquadCoefficients>, std::vector<BiquadCoefficients>>
        linkwitzRileyCrossover(float sampleRate, float crossoverFreq, int order);

private:
    static constexpr float PI = std::numbers::pi_v<float>;

    static float prewarp(float frequency, float sampleRate);
};

} // namespace SoundBoost
