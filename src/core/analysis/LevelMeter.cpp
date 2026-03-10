// src/core/analysis/LevelMeter.cpp
#include "LevelMeter.hpp"
#include <cmath>
#include <algorithm>

namespace SoundBoost {

void LevelMeter::initialize(int sampleRate, int channels) {
    m_sampleRate = sampleRate;
    m_channels = channels;
    m_peakLevels.resize(channels, 0.0f);
    m_rmsLevels.resize(channels, 0.0f);
}

void LevelMeter::process(const AudioBuffer& buffer) {
    int channels = std::min(buffer.getNumChannels(), m_channels);
    size_t numSamples = buffer.getNumSamples();
    const float* data = buffer.getData();

    for (int ch = 0; ch < channels; ++ch) {
        float peak = 0.0f;
        float sumSquares = 0.0f;

        for (size_t i = 0; i < numSamples; ++i) {
            float sample = std::abs(data[i * channels + ch]);
            peak = std::max(peak, sample);
            sumSquares += sample * sample;
        }

        float rms = std::sqrt(sumSquares / numSamples);

        // Peak hold with decay
        if (peak > m_peakLevels[ch]) {
            m_peakLevels[ch] = peak;
        } else {
            m_peakLevels[ch] -= m_decayRate;
            m_peakLevels[ch] = std::max(0.0f, m_peakLevels[ch]);
        }

        m_rmsLevels[ch] = rms;
    }
}

float LevelMeter::getPeakLevel(int channel) const {
    if (channel < 0 || channel >= static_cast<int>(m_peakLevels.size())) {
        return 0.0f;
    }
    return m_peakLevels[channel];
}

float LevelMeter::getRMSLevel(int channel) const {
    if (channel < 0 || channel >= static_cast<int>(m_rmsLevels.size())) {
        return 0.0f;
    }
    return m_rmsLevels[channel];
}

float LevelMeter::getDBFS(int channel) const {
    float peak = getPeakLevel(channel);
    if (peak < 1e-6f) return -96.0f;
    return 20.0f * std::log10(std::min(peak, 1.0f));
}

void LevelMeter::reset() {
    std::fill(m_peakLevels.begin(), m_peakLevels.end(), 0.0f);
    std::fill(m_rmsLevels.begin(), m_rmsLevels.end(), 0.0f);
}

} // namespace SoundBoost
