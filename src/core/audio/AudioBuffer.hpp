// src/core/audio/AudioBuffer.hpp
#pragma once

#include <vector>
#include <span>
#include <memory>
#include <cstdint>
#include <algorithm>
#include <cmath>

namespace SoundBoost {

class AudioBuffer {
public:
    AudioBuffer() = default;
    AudioBuffer(size_t numSamples, int channels, int sampleRate);
    AudioBuffer(const AudioBuffer& other);
    AudioBuffer(AudioBuffer&& other) noexcept;
    AudioBuffer& operator=(const AudioBuffer& other);
    AudioBuffer& operator=(AudioBuffer&& other) noexcept;
    ~AudioBuffer() = default;

    // Initialization
    void allocate(size_t numSamples, int channels, int sampleRate);
    void clear();
    void resize(size_t numSamples);

    // Data access
    float* getData() { return m_data.data(); }
    const float* getData() const { return m_data.data(); }

    float* getChannelData(int channel);
    const float* getChannelData(int channel) const;

    std::span<float> getChannel(int channel);
    std::span<const float> getChannel(int channel) const;

    float getSample(int channel, size_t sample) const;
    void setSample(int channel, size_t sample, float value);

    // Properties
    size_t getNumSamples() const { return m_numSamples; }
    int getNumChannels() const { return m_channels; }
    int getSampleRate() const { return m_sampleRate; }
    size_t getSize() const { return m_data.size(); }
    bool isEmpty() const { return m_data.empty(); }

    // Operations
    void setData(const float* data, size_t size);
    void copyFrom(const AudioBuffer& other);
    void addFrom(const AudioBuffer& other, float gain = 1.0f);
    void applyGain(float gain);
    void applyGainRamp(float startGain, float endGain);
    void normalize(float targetPeak = 1.0f);

    // Analysis
    float getPeak() const;
    float getRMS() const;
    std::pair<float, float> getPeakLevels() const; // Left, Right
    std::pair<float, float> getRMSLevels() const;

    // Utility
    void interleave(const std::vector<const float*>& channels);
    void deinterleave(std::vector<float*>& channels) const;

private:
    std::vector<float> m_data;
    size_t m_numSamples{0};
    int m_channels{0};
    int m_sampleRate{0};
};

// Aligned audio buffer for SIMD operations
class AlignedAudioBuffer {
public:
    static constexpr size_t ALIGNMENT = 32; // AVX alignment

    AlignedAudioBuffer(size_t numSamples, int channels);
    ~AlignedAudioBuffer();

    float* getData() { return m_data; }
    const float* getData() const { return m_data; }

    size_t getSize() const { return m_size; }

private:
    float* m_data{nullptr};
    size_t m_size{0};
};

} // namespace SoundBoost
