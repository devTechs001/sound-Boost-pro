// src/core/ml/BeatDetector.cpp
#include "BeatDetector.hpp"
#include <algorithm>
#include <cmath>

namespace SoundBoost {

float BeatDetector::detectBPM(const AudioBuffer& buffer) {
    // Simple energy-based BPM detection
    const float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    // Compute energy envelope
    std::vector<float> envelope(numSamples / 1024);
    for (size_t i = 0; i < envelope.size(); ++i) {
        float energy = 0.0f;
        size_t start = i * 1024;
        size_t end = std::min(start + 1024, numSamples);
        for (size_t j = start; j < end; ++j) {
            float sample = data[j * channels];
            energy += sample * sample;
        }
        envelope[i] = energy / (end - start);
    }

    // Find peaks in envelope
    std::vector<float> peaks;
    for (size_t i = 1; i < envelope.size() - 1; ++i) {
        if (envelope[i] > envelope[i-1] && envelope[i] > envelope[i+1]) {
            if (envelope[i] > 0.5f) {
                peaks.push_back(static_cast<float>(i));
            }
        }
    }

    // Estimate BPM from peak intervals
    if (peaks.size() < 2) return 120.0f;

    std::vector<float> intervals;
    for (size_t i = 1; i < peaks.size(); ++i) {
        float interval = peaks[i] - peaks[i-1];
        if (interval > 5 && interval < 50) { // Reasonable range
            intervals.push_back(interval);
        }
    }

    if (intervals.empty()) return 120.0f;

    float avgInterval = std::accumulate(intervals.begin(), intervals.end(), 0.0f) / intervals.size();
    float sampleRate = 48000;
    float hopSize = 1024;

    float bpm = 60.0f * sampleRate / (avgInterval * hopSize);
    return std::clamp(bpm, 60.0f, 200.0f);
}

} // namespace SoundBoost
