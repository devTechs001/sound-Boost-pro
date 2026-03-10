// src/core/ml/BeatDetector.cpp
#include "BeatDetector.hpp"
#include "../audio/AudioBuffer.hpp"
#include "../dsp/FFTProcessor.hpp"
#include "../../utils/Logger.hpp"

#include <cmath>
#include <algorithm>
#include <numeric>
#include <deque>

namespace SoundBoost {

BeatDetector::BeatDetector() = default;
BeatDetector::~BeatDetector() = default;

bool BeatDetector::loadModel(const std::string& modelPath) {
    m_modelLoaded = true;
    LOG_INFO("Beat detector initialized");
    return true;
}

float BeatDetector::detectBPM(const AudioBuffer& buffer) {
    if (!m_initialized) return 120.0f;

    const float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    // Simple energy-based BPM detection
    std::vector<float> envelope(numSamples / 512);
    for (size_t i = 0; i < envelope.size(); ++i) {
        float energy = 0.0f;
        size_t start = i * 512;
        size_t end = std::min(start + 512, numSamples);
        for (size_t j = start; j < end; ++j) {
            float sample = data[j * channels];
            energy += sample * sample;
        }
        envelope[i] = std::sqrt(energy / (end - start));
    }

    // Find peaks
    std::vector<float> peaks;
    for (size_t i = 1; i < envelope.size() - 1; ++i) {
        if (envelope[i] > envelope[i-1] && envelope[i] > envelope[i+1]) {
            if (envelope[i] > 0.5f) {
                peaks.push_back(static_cast<float>(i));
            }
        }
    }

    if (peaks.size() < 2) return 120.0f;

    // Calculate intervals
    std::vector<float> intervals;
    for (size_t i = 1; i < peaks.size(); ++i) {
        float interval = peaks[i] - peaks[i-1];
        if (interval > 5 && interval < 50) {
            intervals.push_back(interval);
        }
    }

    if (intervals.empty()) return 120.0f;

    float avgInterval = std::accumulate(intervals.begin(), intervals.end(), 0.0f) / intervals.size();
    float bpm = 60.0f * 48000.0f / (avgInterval * 512);
    return std::clamp(bpm, 60.0f, 200.0f);
}

std::vector<float> BeatDetector::getBeatPositions() const {
    return m_beatPositions;
}

} // namespace SoundBoost
