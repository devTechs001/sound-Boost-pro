// src/core/effects/Compressor.cpp
#include "Compressor.hpp"
#include "../audio/AudioBuffer.hpp"
#include "../../utils/MathUtils.hpp"

#include <cmath>
#include <algorithm>

namespace SoundBoost {

Compressor::Compressor() {
    registerParameter("threshold", -20.0f, -60.0f, 0.0f, "dB");
    registerParameter("ratio", 4.0f, 1.0f, 20.0f, ":1");
    registerParameter("attack", 10.0f, 0.1f, 100.0f, "ms");
    registerParameter("release", 100.0f, 10.0f, 1000.0f, "ms");
    registerParameter("knee", 6.0f, 0.0f, 12.0f, "dB");
    registerParameter("makeup", 0.0f, 0.0f, 18.0f, "dB");
}

void Compressor::initialize(int sampleRate, int bufferSize) {
    m_sampleRate = sampleRate;
    m_envelope = 0.0f;
    m_currentGainReduction = 0.0f;
    m_initialized = true;
}

void Compressor::process(AudioBuffer& buffer) {
    if (!m_enabled || !m_initialized) return;

    float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    // Convert time constants to coefficients
    float attackCoeff = std::exp(-1.0f / (m_attackMs * m_sampleRate / 1000.0f));
    float releaseCoeff = std::exp(-1.0f / (m_releaseMs * m_sampleRate / 1000.0f));

    float thresholdLinear = std::pow(10.0f, m_thresholdDb / 20.0f);
    float kneeLinear = std::pow(10.0f, m_kneeDb / 20.0f);

    for (size_t i = 0; i < numSamples; ++i) {
        // Get max level across channels for this sample
        float maxLevel = 0.0f;
        for (int ch = 0; ch < channels; ++ch) {
            maxLevel = std::max(maxLevel, std::abs(data[i * channels + ch]));
        }

        // Envelope follower (peak detector with smoothing)
        float coeff = (maxLevel > m_envelope) ? attackCoeff : releaseCoeff;
        m_envelope = coeff * m_envelope + (1.0f - coeff) * maxLevel;

        // Compute gain reduction
        float gainReduction = computeGainReduction(m_envelope);
        m_currentGainReduction = gainReduction;

        // Apply gain reduction to all channels
        for (int ch = 0; ch < channels; ++ch) {
            data[i * channels + ch] *= gainReduction;
        }
    }

    // Apply makeup gain
    if (m_makeupGainDb > 0.0f) {
        float makeupLinear = std::pow(10.0f, m_makeupGainDb / 20.0f);
        buffer.applyGain(makeupLinear);
    }
}

float Compressor::computeGainReduction(float inputLevel) {
    if (inputLevel < 1e-6f) return 1.0f;

    float inputDb = 20.0f * std::log10(inputLevel);
    float threshold = m_thresholdDb;
    float knee = m_kneeDb;

    float gainDb;

    if (knee > 0.0f && inputDb > threshold - knee && inputDb < threshold + knee) {
        // Soft knee - smooth transition
        float x = (inputDb - threshold) / knee;
        float ratio = m_ratio;
        gainDb = inputDb + ((-1.0f + 1.0f / ratio) * knee * 0.25f * x * x);
    } else if (inputDb > threshold) {
        // Above threshold - apply compression
        float overDb = inputDb - threshold;
        gainDb = threshold + overDb / m_ratio;
    } else {
        // Below threshold - no compression
        gainDb = inputDb;
    }

    // Convert back to linear gain
    float gainReductionDb = gainDb - inputDb;
    return std::pow(10.0f, gainReductionDb / 20.0f);
}

void Compressor::reset() {
    m_envelope = 0.0f;
    m_currentGainReduction = 0.0f;
}

void Compressor::setThreshold(float thresholdDb) {
    m_thresholdDb = std::clamp(thresholdDb, -60.0f, 0.0f);
    setParameter("threshold", m_thresholdDb);
}

void Compressor::setRatio(float ratio) {
    m_ratio = std::clamp(ratio, 1.0f, 20.0f);
    setParameter("ratio", m_ratio);
}

void Compressor::setAttack(float attackMs) {
    m_attackMs = std::clamp(attackMs, 0.1f, 100.0f);
    setParameter("attack", m_attackMs);
}

void Compressor::setRelease(float releaseMs) {
    m_releaseMs = std::clamp(releaseMs, 10.0f, 1000.0f);
    setParameter("release", m_releaseMs);
}

void Compressor::setKnee(float kneeDb) {
    m_kneeDb = std::clamp(kneeDb, 0.0f, 12.0f);
    setParameter("knee", m_kneeDb);
}

void Compressor::setMakeupGain(float gainDb) {
    m_makeupGainDb = std::clamp(gainDb, 0.0f, 18.0f);
    setParameter("makeup", m_makeupGainDb);
}

} // namespace SoundBoost
