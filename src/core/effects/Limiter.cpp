// src/core/effects/Limiter.cpp
#include "Limiter.hpp"
#include <cmath>
#include <algorithm>

namespace SoundBoost {

Limiter::Limiter() {
    registerParameter("ceiling", -0.3f, -12.0f, 0.0f, "dB");
    registerParameter("release", 50.0f, 1.0f, 500.0f, "ms");
    registerParameter("lookahead", 1.0f, 0.0f, 5.0f, "ms");
}

void Limiter::initialize(int sampleRate, int bufferSize) {
    m_sampleRate = sampleRate;
    m_numChannels = 2;

    // Lookahead buffer (5ms max)
    size_t maxLookahead = static_cast<size_t>(0.005f * sampleRate);
    m_lookaheadBuffer.resize(m_numChannels);
    for (auto& buf : m_lookaheadBuffer) {
        buf.resize(maxLookahead, 0.0f);
    }
    m_lookaheadWritePos = 0;

    updateCoefficients();
    m_initialized = true;
}

void Limiter::updateCoefficients() {
    m_ceilingLinear = std::pow(10.0f, m_ceiling / 20.0f);
    m_releaseCoeff = std::exp(-1.0f / (m_release * 0.001f * m_sampleRate));
    m_lookaheadSamples = static_cast<size_t>(m_lookahead * 0.001f * m_sampleRate);
}

float Limiter::findTruePeak(const float* samples, size_t count) {
    float maxPeak = 0.0f;

    // 4x oversampling for true peak detection
    for (size_t i = 0; i < count - 1; ++i) {
        float s0 = samples[i];
        float s1 = samples[i + 1];

        // Find peak in interpolated samples
        for (int j = 0; j < 4; ++j) {
            float t = j / 4.0f;
            // Simple linear interpolation (could use sinc for better accuracy)
            float interp = s0 + (s1 - s0) * t;
            maxPeak = std::max(maxPeak, std::abs(interp));
        }
    }

    return maxPeak;
}

void Limiter::process(AudioBuffer& buffer) {
    if (!m_enabled || !m_initialized) return;

    float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    // Adjust buffers if channels changed
    if (channels != m_numChannels) {
        m_numChannels = channels;
        size_t bufSize = m_lookaheadBuffer[0].size();
        m_lookaheadBuffer.resize(channels);
        for (auto& buf : m_lookaheadBuffer) {
            buf.resize(bufSize, 0.0f);
        }
    }

    float maxGR = 0.0f;

    for (size_t i = 0; i < numSamples; ++i) {
        // Find peak across all channels
        float peak = 0.0f;
        for (int ch = 0; ch < channels; ++ch) {
            peak = std::max(peak, std::abs(data[i * channels + ch]));
        }

        // True peak detection
        if (m_truePeak && i > 0) {
            // Check interpolated samples
            for (int ch = 0; ch < channels; ++ch) {
                float prev = data[(i - 1) * channels + ch];
                float curr = data[i * channels + ch];
                float interPeak = std::max(std::abs(prev), std::abs(curr));
                // Estimate interpolated peak
                float midpoint = std::abs((prev + curr) / 2.0f);
                interPeak = std::max(interPeak, midpoint * 1.1f); // Approximation
                peak = std::max(peak, interPeak);
            }
        }

        // Calculate required gain
        float targetGain = 1.0f;
        if (peak > m_ceilingLinear) {
            targetGain = m_ceilingLinear / peak;
        }

        // Smooth gain changes
        if (targetGain < m_currentGain) {
            // Instant attack for limiting
            m_currentGain = targetGain;
        } else {
            // Smooth release
            m_currentGain = m_releaseCoeff * m_currentGain + (1.0f - m_releaseCoeff) * targetGain;
        }

        // Track gain reduction
        float gr = 1.0f - m_currentGain;
        maxGR = std::max(maxGR, gr);

        // Apply gain
        for (int ch = 0; ch < channels; ++ch) {
            size_t idx = i * channels + ch;
            data[idx] *= m_currentGain;

            // Hard clip as safety (should rarely trigger)
            data[idx] = std::clamp(data[idx], -m_ceilingLinear, m_ceilingLinear);
        }
    }

    m_gainReduction.store(maxGR);
}

void Limiter::reset() {
    m_currentGain = 1.0f;
    m_gainReduction.store(0.0f);
    m_lookaheadWritePos = 0;
    for (auto& buf : m_lookaheadBuffer) {
        std::fill(buf.begin(), buf.end(), 0.0f);
    }
}

void Limiter::setCeiling(float ceilingDb) {
    m_ceiling = std::clamp(ceilingDb, -12.0f, 0.0f);
    setParameter("ceiling", m_ceiling);
    updateCoefficients();
}

void Limiter::setRelease(float releaseMs) {
    m_release = std::clamp(releaseMs, 1.0f, 500.0f);
    setParameter("release", m_release);
    updateCoefficients();
}

void Limiter::setLookahead(float lookaheadMs) {
    m_lookahead = std::clamp(lookaheadMs, 0.0f, 5.0f);
    updateCoefficients();
}

void Limiter::setTruePeak(bool enabled) {
    m_truePeak = enabled;
}

} // namespace SoundBoost
