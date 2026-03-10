// src/core/effects/Compressor.cpp
#include "Compressor.hpp"
#include "../../utils/MathUtils.hpp"

#include <cmath>
#include <algorithm>

namespace SoundBoost {

Compressor::Compressor() {
    registerParameter("threshold", -20.0f, -60.0f, 0.0f, "dB");
    registerParameter("ratio", 4.0f, 1.0f, 20.0f, ":1");
    registerParameter("attack", 10.0f, 0.1f, 100.0f, "ms");
    registerParameter("release", 100.0f, 10.0f, 1000.0f, "ms");
    registerParameter("knee", 6.0f, 0.0f, 24.0f, "dB");
    registerParameter("makeup", 0.0f, 0.0f, 24.0f, "dB");
}

void Compressor::initialize(int sampleRate, int bufferSize) {
    m_sampleRate = sampleRate;

    // Initialize lookahead buffer (max 10ms)
    size_t maxLookahead = static_cast<size_t>(0.01f * sampleRate * 2); // stereo
    m_lookaheadBuffer.resize(maxLookahead, 0.0f);
    m_lookaheadWritePos = 0;

    updateCoefficients();
    m_initialized = true;
}

void Compressor::updateCoefficients() {
    // Attack and release coefficients
    m_attackCoeff = std::exp(-1.0f / (m_attack * 0.001f * m_sampleRate));
    m_releaseCoeff = std::exp(-1.0f / (m_release * 0.001f * m_sampleRate));

    // Lookahead samples
    m_lookaheadSamples = static_cast<size_t>(m_lookahead * 0.001f * m_sampleRate);

    // Makeup gain
    if (m_autoMakeup) {
        // Estimate average gain reduction
        float avgReduction = (m_threshold * (1.0f - 1.0f / m_ratio)) / 2.0f;
        m_makeupLinear = std::pow(10.0f, -avgReduction / 20.0f);
    } else {
        m_makeupLinear = std::pow(10.0f, m_makeupGain / 20.0f);
    }
}

float Compressor::softKnee(float inputDb, float thresholdDb, float kneeDb, float ratio) {
    float halfKnee = kneeDb / 2.0f;

    if (inputDb < thresholdDb - halfKnee) {
        // Below knee
        return inputDb;
    } else if (inputDb > thresholdDb + halfKnee) {
        // Above knee
        return thresholdDb + (inputDb - thresholdDb) / ratio;
    } else {
        // In knee region - smooth transition
        float x = inputDb - thresholdDb + halfKnee;
        float compressionCurve = (1.0f / ratio - 1.0f) / (2.0f * kneeDb);
        return inputDb + compressionCurve * x * x;
    }
}

float Compressor::computeGain(float inputLevel) {
    // Convert to dB
    float inputDb = (inputLevel > 1e-10f) ? 20.0f * std::log10(inputLevel) : -100.0f;

    // Apply compression curve
    float outputDb;
    if (m_knee > 0.0f) {
        outputDb = softKnee(inputDb, m_threshold, m_knee, m_ratio);
    } else {
        // Hard knee
        if (inputDb > m_threshold) {
            outputDb = m_threshold + (inputDb - m_threshold) / m_ratio;
        } else {
            outputDb = inputDb;
        }
    }

    // Calculate gain reduction
    float gainReductionDb = outputDb - inputDb;
    return std::pow(10.0f, gainReductionDb / 20.0f);
}

void Compressor::process(AudioBuffer& buffer) {
    if (!m_enabled || !m_initialized) return;

    float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    float maxInputLevel = 0.0f;
    float maxOutputLevel = 0.0f;
    float maxGainReduction = 0.0f;

    for (size_t i = 0; i < numSamples; ++i) {
        // Calculate input level (max of all channels)
        float inputLevel = 0.0f;
        for (int ch = 0; ch < channels; ++ch) {
            inputLevel = std::max(inputLevel, std::abs(data[i * channels + ch]));
        }
        maxInputLevel = std::max(maxInputLevel, inputLevel);

        // Envelope follower (peak detection)
        float targetEnv = inputLevel;
        if (targetEnv > m_envelope) {
            m_envelope = m_attackCoeff * m_envelope + (1.0f - m_attackCoeff) * targetEnv;
        } else {
            m_envelope = m_releaseCoeff * m_envelope + (1.0f - m_releaseCoeff) * targetEnv;
        }

        // Compute gain
        float gain = computeGain(m_envelope);

        // Apply makeup gain
        gain *= m_makeupLinear;

        // Track gain reduction
        float gr = 1.0f - gain / m_makeupLinear;
        maxGainReduction = std::max(maxGainReduction, gr);

        // Apply gain to all channels
        for (int ch = 0; ch < channels; ++ch) {
            size_t idx = i * channels + ch;
            data[idx] *= gain;
            maxOutputLevel = std::max(maxOutputLevel, std::abs(data[idx]));
        }
    }

    // Update meters
    m_inputLevel.store(maxInputLevel);
    m_outputLevel.store(maxOutputLevel);
    m_currentGainReduction.store(maxGainReduction);
}

void Compressor::reset() {
    m_envelope = 0.0f;
    std::fill(m_lookaheadBuffer.begin(), m_lookaheadBuffer.end(), 0.0f);
    m_lookaheadWritePos = 0;
    m_currentGainReduction.store(0.0f);
    m_inputLevel.store(0.0f);
    m_outputLevel.store(0.0f);
}

void Compressor::setThreshold(float thresholdDb) {
    m_threshold = std::clamp(thresholdDb, -60.0f, 0.0f);
    setParameter("threshold", m_threshold);
    updateCoefficients();
}

void Compressor::setRatio(float ratio) {
    m_ratio = std::clamp(ratio, 1.0f, 20.0f);
    setParameter("ratio", m_ratio);
    updateCoefficients();
}

void Compressor::setAttack(float attackMs) {
    m_attack = std::clamp(attackMs, 0.1f, 100.0f);
    setParameter("attack", m_attack);
    updateCoefficients();
}

void Compressor::setRelease(float releaseMs) {
    m_release = std::clamp(releaseMs, 10.0f, 1000.0f);
    setParameter("release", m_release);
    updateCoefficients();
}

void Compressor::setKnee(float kneeDb) {
    m_knee = std::clamp(kneeDb, 0.0f, 24.0f);
    setParameter("knee", m_knee);
}

void Compressor::setMakeupGain(float gainDb) {
    m_makeupGain = std::clamp(gainDb, 0.0f, 24.0f);
    setParameter("makeup", m_makeupGain);
    updateCoefficients();
}

void Compressor::setAutoMakeup(bool enabled) {
    m_autoMakeup = enabled;
    updateCoefficients();
}

void Compressor::setLookahead(float lookaheadMs) {
    m_lookahead = std::clamp(lookaheadMs, 0.0f, 10.0f);
    updateCoefficients();
}

} // namespace SoundBoost
