// src/core/effects/StereoWidener.cpp
#include "StereoWidener.hpp"
#include "../audio/AudioBuffer.hpp"
#include "../dsp/FilterDesign.hpp"

#include <cmath>

namespace SoundBoost {

StereoWidener::StereoWidener() {
    registerParameter("width", 1.0f, 0.0f, 2.0f, "");
    registerParameter("mix", 1.0f, 0.0f, 1.0f, "");
}

void StereoWidener::initialize(int sampleRate, int bufferSize) {
    // Initialize mid-side processing filters
    // Simple all-pass for phase manipulation
    BiquadCoefficients coeffs;
    coeffs.b0 = 1.0f;
    coeffs.b1 = 0.0f;
    coeffs.b2 = 0.0f;
    coeffs.a1 = 0.0f;
    coeffs.a2 = 0.0f;

    m_leftFilter.setCoefficients(coeffs);
    m_rightFilter.setCoefficients(coeffs);

    m_initialized = true;
}

void StereoWidener::process(AudioBuffer& buffer) {
    if (!m_enabled || !m_initialized) return;

    int channels = buffer.getNumChannels();
    if (channels < 2) return; // Need stereo

    float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();

    for (size_t i = 0; i < numSamples; ++i) {
        size_t leftIdx = i * channels;
        size_t rightIdx = i * channels + 1;

        float L = data[leftIdx];
        float R = data[rightIdx];

        // Mid-Side processing
        float M = (L + R) * 0.5f;  // Mid (sum)
        float S = (L - R) * 0.5f;  // Side (difference)

        // Apply width to side channel
        S *= m_width;

        // Convert back to left-right
        float Lout = M + S;
        float Rout = M - S;

        // Apply mix
        data[leftIdx] = L * (1.0f - m_mix) + Lout * m_mix;
        data[rightIdx] = R * (1.0f - m_mix) + Rout * m_mix;
    }
}

void StereoWidener::reset() {
    m_leftFilter.reset();
    m_rightFilter.reset();
}

void StereoWidener::setWidth(float width) {
    m_width = std::clamp(width, 0.0f, 2.0f);
    setParameter("width", m_width);
}

void StereoWidener::setMix(float mix) {
    m_mix = std::clamp(mix, 0.0f, 1.0f);
    setParameter("mix", m_mix);
}

} // namespace SoundBoost
