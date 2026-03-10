// src/core/effects/Equalizer.cpp
#include "Equalizer.hpp"
#include "../dsp/FilterDesign.hpp"
#include "../../utils/Logger.hpp"

#include <algorithm>

namespace SoundBoost {

Equalizer::Equalizer(int numBands)
    : m_numBands(numBands)
{
    auto presetFreqs = getPresetFrequencies();

    m_frequencies.resize(m_numBands);
    m_gains.resize(m_numBands, 0.0f);
    m_qValues.resize(m_numBands, 1.0f);

    for (int i = 0; i < m_numBands && i < static_cast<int>(presetFreqs.size()); ++i) {
        m_frequencies[i] = presetFreqs[i];
    }

    // Register parameters for each band
    for (int i = 0; i < m_numBands; ++i) {
        registerParameter("gain_" + std::to_string(i), 0.0f, -18.0f, 18.0f, "dB");
    }
}

void Equalizer::initialize(int sampleRate, int bufferSize) {
    m_sampleRate = sampleRate;

    // Create filters for each band and channel (stereo)
    m_filters.resize(2); // Left and right
    for (auto& channelFilters : m_filters) {
        channelFilters.resize(m_numBands);
    }

    updateFilters();
    m_initialized = true;

    LOG_DEBUG("Equalizer initialized with {} bands", m_numBands);
}

void Equalizer::process(AudioBuffer& buffer) {
    if (!m_enabled || !m_initialized) return;

    int channels = buffer.getNumChannels();
    float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();

    // Process each channel
    for (int ch = 0; ch < std::min(channels, 2); ++ch) {
        for (size_t i = 0; i < numSamples; ++i) {
            size_t idx = i * channels + ch;
            float sample = data[idx];

            // Process through all EQ bands
            for (int band = 0; band < m_numBands; ++band) {
                sample = m_filters[ch][band].process(sample);
            }

            data[idx] = sample;
        }
    }
}

void Equalizer::reset() {
    for (auto& channelFilters : m_filters) {
        for (auto& filter : channelFilters) {
            filter.reset();
        }
    }
}

void Equalizer::updateFilters() {
    for (int ch = 0; ch < static_cast<int>(m_filters.size()); ++ch) {
        for (int band = 0; band < m_numBands; ++band) {
            float freq = m_frequencies[band];
            float gain = m_gains[band];
            float q = m_qValues[band];

            // Choose filter type based on frequency
            BiquadCoefficients coeffs;
            if (gain >= 0.0f) {
                // Boost - use peak EQ
                coeffs = FilterDesign::peakEQ(m_sampleRate, freq, gain, q);
            } else {
                // Cut - use notch for narrow cuts, peak for wide
                if (q > 2.0f) {
                    coeffs = FilterDesign::notch(m_sampleRate, freq, q);
                } else {
                    coeffs = FilterDesign::peakEQ(m_sampleRate, freq, gain, q);
                }
            }

            m_filters[ch][band].setCoefficients(coeffs);
        }
    }
}

void Equalizer::setNumBands(int numBands) {
    m_numBands = numBands;
    m_frequencies.resize(numBands);
    m_gains.resize(numBands, 0.0f);
    m_qValues.resize(numBands, 1.0f);
}

void Equalizer::setBandFrequency(int band, float frequency) {
    if (band >= 0 && band < m_numBands) {
        m_frequencies[band] = std::clamp(frequency, 20.0f, 20000.0f);
        updateFilters();
    }
}

float Equalizer::getBandFrequency(int band) const {
    if (band >= 0 && band < m_numBands) {
        return m_frequencies[band];
    }
    return 1000.0f;
}

void Equalizer::setBandGain(int band, float gainDb) {
    if (band >= 0 && band < m_numBands) {
        m_gains[band] = std::clamp(gainDb, -18.0f, 18.0f);
        setParameter("gain_" + std::to_string(band), m_gains[band]);
        updateFilters();
    }
}

float Equalizer::getBandGain(int band) const {
    if (band >= 0 && band < m_numBands) {
        return m_gains[band];
    }
    return 0.0f;
}

void Equalizer::setBandQ(int band, float q) {
    if (band >= 0 && band < m_numBands) {
        m_qValues[band] = std::clamp(q, 0.1f, 10.0f);
        updateFilters();
    }
}

float Equalizer::getBandQ(int band) const {
    if (band >= 0 && band < m_numBands) {
        return m_qValues[band];
    }
    return 1.0f;
}

} // namespace SoundBoost
