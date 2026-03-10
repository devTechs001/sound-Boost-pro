// src/core/dsp/BiquadFilter.hpp
#pragma once

#include <array>
#include <vector>

namespace SoundBoost {

struct BiquadCoefficients {
    float b0{1.0f}, b1{0.0f}, b2{0.0f};
    float a1{0.0f}, a2{0.0f};
};

class BiquadFilter {
public:
    BiquadFilter() = default;
    explicit BiquadFilter(const BiquadCoefficients& coeffs);

    void setCoefficients(const BiquadCoefficients& coeffs);
    const BiquadCoefficients& getCoefficients() const { return m_coeffs; }

    float process(float input);
    void processBlock(float* data, size_t numSamples);
    void processBlock(const float* input, float* output, size_t numSamples);

    void reset();

private:
    BiquadCoefficients m_coeffs;

    // State variables (Direct Form II Transposed)
    float m_z1{0.0f};
    float m_z2{0.0f};
};

// Cascaded biquad sections for higher-order filters
class CascadedBiquad {
public:
    CascadedBiquad() = default;
    explicit CascadedBiquad(int numSections);

    void setNumSections(int numSections);
    void setSectionCoefficients(int section, const BiquadCoefficients& coeffs);

    float process(float input);
    void processBlock(float* data, size_t numSamples);

    void reset();

private:
    std::vector<BiquadFilter> m_sections;
};

} // namespace SoundBoost
