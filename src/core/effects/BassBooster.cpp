// src/core/effects/BassBooster.cpp
#include "BassBooster.hpp"
#include "../dsp/FilterDesign.hpp"
#include "../../utils/MathUtils.hpp"

#include <cmath>
#include <algorithm>

namespace SoundBoost {

BassBooster::BassBooster() {
    // Register parameters
    registerParameter("frequency", 80.0f, 20.0f, 200.0f, "Hz");
    registerParameter("gain", 6.0f, 0.0f, 18.0f, "dB");
    registerParameter("width", 1.5f, 0.5f, 4.0f, "Q");
    registerParameter("harmonics", 0.0f, 0.0f, 100.0f, "%");
    registerParameter("subbass", 0.0f, 0.0f, 1.0f, "");
}

void BassBooster::initialize(int sampleRate, int bufferSize) {
    m_sampleRate = sampleRate;
    m_numChannels = 2; // Will be updated based on buffer

    m_lowShelfFilters.resize(m_numChannels);
    m_peakFilters.resize(m_numChannels);
    m_subBassFilters.resize(m_numChannels);
    m_harmonicState.resize(m_numChannels, 0.0f);

    updateFilters();

    m_initialized = true;
}

void BassBooster::process(AudioBuffer& buffer) {
    if (!m_enabled || !m_initialized) return;

    int channels = buffer.getNumChannels();
    if (channels != m_numChannels) {
        m_numChannels = channels;
        m_lowShelfFilters.resize(channels);
        m_peakFilters.resize(channels);
        m_subBassFilters.resize(channels);
        m_harmonicState.resize(channels, 0.0f);
        updateFilters();
    }

    float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();

    // Process each sample
    for (size_t i = 0; i < numSamples; ++i) {
        for (int ch = 0; ch < channels; ++ch) {
            size_t idx = i * channels + ch;
            float sample = data[idx];

            // Apply low shelf boost
            sample = m_lowShelfFilters[ch].process(sample);

            // Apply peak EQ for focused bass
            sample = m_peakFilters[ch].process(sample);

            data[idx] = sample;
        }
    }

    // Add harmonic enhancement if enabled
    if (m_harmonics > 0.0f) {
        processHarmonics(buffer);
    }

    // Add sub-bass synthesis if enabled
    if (m_subBassEnabled) {
        processSubBass(buffer);
    }
}

void BassBooster::updateFilters() {
    // Low shelf filter for broad bass boost
    auto lowShelfCoeffs = FilterDesign::lowShelf(
        m_sampleRate,
        m_frequency * 0.7f,  // Slightly below center
        m_gainDb * 0.6f,     // Partial gain
        1.0f
    );

    // Peak EQ for focused bass at center frequency
    auto peakCoeffs = FilterDesign::peakEQ(
        m_sampleRate,
        m_frequency,
        m_gainDb * 0.5f,
        m_width
    );

    // Sub-bass filter (very low frequency emphasis)
    auto subBassCoeffs = FilterDesign::lowShelf(
        m_sampleRate,
        40.0f,
        6.0f,
        0.7f
    );

    for (int ch = 0; ch < m_numChannels; ++ch) {
        m_lowShelfFilters[ch].setCoefficients(lowShelfCoeffs);
        m_peakFilters[ch].setCoefficients(peakCoeffs);
        m_subBassFilters[ch].setCoefficients(subBassCoeffs);
    }
}

void BassBooster::processHarmonics(AudioBuffer& buffer) {
    float harmonicAmount = m_harmonics / 100.0f;
    float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    for (size_t i = 0; i < numSamples; ++i) {
        for (int ch = 0; ch < channels; ++ch) {
            size_t idx = i * channels + ch;
            float sample = data[idx];

            // Simple harmonic enhancement using soft clipping
            float harmonic = std::tanh(sample * 2.0f) * 0.5f;

            // Mix in harmonics
            data[idx] = sample + harmonic * harmonicAmount;
        }
    }
}

void BassBooster::processSubBass(AudioBuffer& buffer) {
    float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    // Sub-bass synthesis using octave divider principle
    for (size_t i = 0; i < numSamples; ++i) {
        for (int ch = 0; ch < channels; ++ch) {
            size_t idx = i * channels + ch;
            float sample = data[idx];

            // Apply sub-bass filter
            float subBass = m_subBassFilters[ch].process(sample);

            // Full-wave rectification for octave-down effect
            subBass = std::abs(subBass) * 0.3f;

            // Mix back
            data[idx] = sample + subBass;
        }
    }
}

void BassBooster::reset() {
    for (auto& filter : m_lowShelfFilters) {
        filter.reset();
    }
    for (auto& filter : m_peakFilters) {
        filter.reset();
    }
    for (auto& filter : m_subBassFilters) {
        filter.reset();
    }
    std::fill(m_harmonicState.begin(), m_harmonicState.end(), 0.0f);
}

void BassBooster::setFrequency(float frequency) {
    m_frequency = std::clamp(frequency, 20.0f, 200.0f);
    setParameter("frequency", m_frequency);
    updateFilters();
}

void BassBooster::setGain(float gainDb) {
    m_gainDb = std::clamp(gainDb, 0.0f, 18.0f);
    setParameter("gain", m_gainDb);
    updateFilters();
}

void BassBooster::setWidth(float width) {
    m_width = std::clamp(width, 0.5f, 4.0f);
    setParameter("width", m_width);
    updateFilters();
}

void BassBooster::setHarmonics(float amount) {
    m_harmonics = std::clamp(amount, 0.0f, 100.0f);
    setParameter("harmonics", m_harmonics);
}

void BassBooster::setSubBass(bool enabled) {
    m_subBassEnabled = enabled;
    setParameter("subbass", enabled ? 1.0f : 0.0f);
}

} // namespace SoundBoost
