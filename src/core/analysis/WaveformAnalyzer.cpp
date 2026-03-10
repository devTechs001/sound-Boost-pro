// src/core/analysis/WaveformAnalyzer.cpp
#include "WaveformAnalyzer.hpp"
#include <cmath>
#include <algorithm>

namespace SoundBoost {

void WaveformAnalyzer::initialize(int sampleRate) {
    m_sampleRate = sampleRate;
}

void WaveformAnalyzer::process(const AudioBuffer& buffer) {
    const float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    // Downsample for visualization
    size_t targetSize = 1000;
    m_waveformData.resize(targetSize);

    size_t step = std::max(size_t(1), numSamples / targetSize);
    for (size_t i = 0; i < targetSize && i * step < numSamples; ++i) {
        m_waveformData[i] = data[i * step * channels];
    }

    // Calculate zero crossing rate
    int crossings = 0;
    for (size_t i = 1; i < numSamples; ++i) {
        if ((data[(i-1) * channels] >= 0 && data[i * channels] < 0) ||
            (data[(i-1) * channels] < 0 && data[i * channels] >= 0)) {
            ++crossings;
        }
    }
    m_zcr = static_cast<float>(crossings) / numSamples;

    // Calculate crest factor
    float peak = buffer.getPeak();
    float rms = buffer.getRMS();
    m_crestFactor = rms > 0 ? peak / rms : 0.0f;
}

} // namespace SoundBoost
