// src/core/effects/Limiter.cpp
#include "Limiter.hpp"
#include "../audio/AudioBuffer.hpp"
#include "../dsp/SIMD/SIMDDispatcher.hpp"

#include <cmath>
#include <algorithm>

namespace SoundBoost {

Limiter::Limiter() {
    registerParameter("ceiling", -0.3f, -12.0f, 0.0f, "dB");
    registerParameter("lookahead", 5.0f, 0.0f, 20.0f, "ms");
    registerParameter("release", 50.0f, 10.0f, 500.0f, "ms");
}

void Limiter::initialize(int sampleRate, int bufferSize) {
    m_sampleRate = sampleRate;
    m_initialized = true;
}

void Limiter::process(AudioBuffer& buffer) {
    if (!m_enabled || !m_initialized) return;

    float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    float ceilingLinear = std::pow(10.0f, m_ceilingDb / 20.0f);
    float releaseCoeff = std::exp(-1.0f / (m_releaseMs * m_sampleRate / 1000.0f));

    static float releaseState = 0.0f;

    for (size_t i = 0; i < numSamples; ++i) {
        // Find peak in this sample across all channels
        float peak = 0.0f;
        for (int ch = 0; ch < channels; ++ch) {
            peak = std::max(peak, std::abs(data[i * channels + ch]));
        }

        // Calculate required gain to keep peak below ceiling
        float gain = 1.0f;
        if (peak > ceilingLinear) {
            gain = ceilingLinear / peak;
        }

        // Smooth release
        if (gain > releaseState) {
            releaseState = gain;
        } else {
            releaseState = releaseCoeff * releaseState + (1.0f - releaseCoeff) * gain;
        }

        // Apply limiting
        for (int ch = 0; ch < channels; ++ch) {
            data[i * channels + ch] *= releaseState;
        }
    }

    // Hard clip as final safety
    SIMD::clip(data, -ceilingLinear, ceilingLinear, numSamples * channels);
}

void Limiter::reset() {
}

void Limiter::setCeiling(float ceilingDb) {
    m_ceilingDb = std::clamp(ceilingDb, -12.0f, 0.0f);
    setParameter("ceiling", m_ceilingDb);
}

void Limiter::setLookahead(float lookaheadMs) {
    m_lookaheadMs = std::clamp(lookaheadMs, 0.0f, 20.0f);
    setParameter("lookahead", m_lookaheadMs);
}

void Limiter::setRelease(float releaseMs) {
    m_releaseMs = std::clamp(releaseMs, 10.0f, 500.0f);
    setParameter("release", m_releaseMs);
}

} // namespace SoundBoost
