// src/core/analysis/SpectrumAnalyzer.cpp
#include "SpectrumAnalyzer.hpp"
#include "../dsp/FFTProcessor.hpp"

#include <cmath>

namespace SoundBoost {

void SpectrumAnalyzer::initialize(int sampleRate, size_t fftSize) {
    m_sampleRate = sampleRate;
    m_fftSize = fftSize;
    m_magnitudes.resize(fftSize / 2 + 1, 0.0f);
    computeFrequencies();
}

void SpectrumAnalyzer::process(const AudioBuffer& buffer) {
    FFTProcessor fft;
    fft.initialize(m_fftSize);
    m_magnitudes = fft.computeMagnitudeSpectrum(buffer);
}

void SpectrumAnalyzer::computeFrequencies() {
    m_frequencies.resize(m_magnitudes.size());
    float binWidth = static_cast<float>(m_sampleRate) / m_fftSize;

    for (size_t i = 0; i < m_frequencies.size(); ++i) {
        m_frequencies[i] = i * binWidth;
    }
}

float SpectrumAnalyzer::getFrequencyBin(int index) const {
    if (index < 0 || index >= static_cast<int>(m_frequencies.size())) {
        return 0.0f;
    }
    return m_frequencies[index];
}

float SpectrumAnalyzer::getMagnitudeAtFrequency(float freq) const {
    float binWidth = static_cast<float>(m_sampleRate) / m_fftSize;
    int index = static_cast<int>(freq / binWidth);

    if (index < 0 || index >= static_cast<int>(m_magnitudes.size())) {
        return 0.0f;
    }

    return m_magnitudes[index];
}

} // namespace SoundBoost
