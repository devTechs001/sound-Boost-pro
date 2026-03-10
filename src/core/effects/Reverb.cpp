// src/core/effects/Reverb.cpp
#include "Reverb.hpp"

namespace SoundBoost {

// AllpassFilter implementation
AllpassFilter::AllpassFilter(int maxDelay) : m_buffer(maxDelay, 0.0f) {}

void AllpassFilter::setDelay(int samples) { m_delay = samples; }

void AllpassFilter::setFeedback(float feedback) { m_feedback = feedback; }

float AllpassFilter::process(float input) {
    float buffered = m_buffer[m_writeIndex];
    float output = -input + buffered;
    m_buffer[m_writeIndex] = input + buffered * m_feedback;
    m_writeIndex = (m_writeIndex + 1) % m_buffer.size();
    return output;
}

void AllpassFilter::clear() { std::fill(m_buffer.begin(), m_buffer.end(), 0.0f); }

// CombFilter implementation
CombFilter::CombFilter(int maxDelay) : m_buffer(maxDelay, 0.0f), m_filter(maxDelay, 0.0f) {}

void CombFilter::setDelay(int samples) { m_delay = samples; }

void CombFilter::setFeedback(float feedback) { m_feedback = feedback; }

void CombFilter::setDamping(float damping) { m_damping = damping; }

float CombFilter::process(float input) {
    float output = m_buffer[m_writeIndex];
    m_filterStore = output * (1.0f - m_damping) + m_filterStore * m_damping;
    m_buffer[m_writeIndex] = input + m_filterStore * m_feedback;
    m_writeIndex = (m_writeIndex + 1) % m_buffer.size();
    return output;
}

void CombFilter::clear() {
    std::fill(m_buffer.begin(), m_buffer.end(), 0.0f);
    std::fill(m_filter.begin(), m_filter.end(), 0.0f);
    m_filterStore = 0.0f;
}

// Reverb implementation
Reverb::Reverb() {
    registerParameter("roomSize", 0.5f, 0.0f, 1.0f, "");
    registerParameter("damping", 0.5f, 0.0f, 1.0f, "");
    registerParameter("wet", 0.33f, 0.0f, 1.0f, "");
    registerParameter("dry", 0.67f, 0.0f, 1.0f, "");
    registerParameter("width", 1.0f, 0.0f, 1.0f, "");
}

void Reverb::initialize(int sampleRate, int bufferSize) {
    m_sampleRate = sampleRate;

    // Scale delays based on sample rate
    float scale = sampleRate / 44100.0f;

    // Initialize comb filters (Schroeder reverberator)
    constexpr int combTunings[NUM_COMBS] = {1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617};
    for (int i = 0; i < NUM_COMBS; ++i) {
        m_combFilters[i].setDelay(static_cast<int>(combTunings[i] * scale));
        m_combFilters[i].setFeedback(0.84f);
        m_combFilters[i].setDamping(m_damping);
    }

    // Initialize allpass filters
    constexpr int allpassTunings[NUM_ALLPASSES] = {556, 441, 341, 225};
    for (int i = 0; i < NUM_ALLPASSES; ++i) {
        m_allpassFilters[i].setDelay(static_cast<int>(allpassTunings[i] * scale));
        m_allpassFilters[i].setFeedback(0.5f);
    }

    m_initialized = true;
}

void Reverb::process(AudioBuffer& buffer) {
    if (!m_enabled || !m_initialized) return;

    float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    for (size_t i = 0; i < numSamples; ++i) {
        float input = 0.0f;
        for (int ch = 0; ch < channels; ++ch) {
            input += data[i * channels + ch];
        }
        input /= channels;

        if (m_freeze) {
            for (auto& comb : m_combFilters) comb.setFeedback(1.0f);
        } else {
            for (auto& comb : m_combFilters) comb.setFeedback(0.84f);
        }

        // Parallel comb filters
        float output = 0.0f;
        for (auto& comb : m_combFilters) {
            output += comb.process(input);
        }

        // Series allpass filters
        for (auto& allpass : m_allpassFilters) {
            output = allpass.process(output);
        }

        // Apply wet/dry mix and width
        float wet = output * m_wetLevel;
        float dry = input * m_dryLevel;

        for (int ch = 0; ch < channels; ++ch) {
            size_t idx = i * channels + ch;
            if (channels == 2) {
                // Stereo: apply width
                float left = (ch == 0) ? (dry + wet * (1.0f - m_width)) : (dry + wet * (1.0f + m_width));
                float right = (ch == 1) ? (dry + wet * (1.0f + m_width)) : (dry + wet * (1.0f - m_width));
                data[idx] = (ch == 0) ? left : right;
            } else {
                data[idx] = dry + wet;
            }
        }
    }
}

void Reverb::reset() {
    for (auto& comb : m_combFilters) comb.clear();
    for (auto& allpass : m_allpassFilters) allpass.clear();
}

void Reverb::setRoomSize(float size) {
    m_roomSize = std::clamp(size, 0.0f, 1.0f);
    setParameter("roomSize", m_roomSize);
    for (auto& comb : m_combFilters) {
        comb.setFeedback(0.84f + m_roomSize * 0.16f);
    }
}

void Reverb::setDamping(float damping) {
    m_damping = std::clamp(damping, 0.0f, 1.0f);
    setParameter("damping", m_damping);
    for (auto& comb : m_combFilters) {
        comb.setDamping(m_damping);
    }
}

void Reverb::setWetLevel(float level) {
    m_wetLevel = std::clamp(level, 0.0f, 1.0f);
    setParameter("wet", m_wetLevel);
}

void Reverb::setDryLevel(float level) {
    m_dryLevel = std::clamp(level, 0.0f, 1.0f);
    setParameter("dry", m_dryLevel);
}

void Reverb::setWidth(float width) {
    m_width = std::clamp(width, 0.0f, 1.0f);
    setParameter("width", m_width);
}

void Reverb::setFreeze(bool freeze) {
    m_freeze = freeze;
}

} // namespace SoundBoost
