// src/core/audio/AudioBuffer.cpp
#include "AudioBuffer.hpp"
#include "../dsp/SIMD/SIMDDispatcher.hpp"
#include <stdexcept>
#include <numeric>

namespace SoundBoost {

AudioBuffer::AudioBuffer(size_t numSamples, int channels, int sampleRate)
    : m_numSamples(numSamples)
    , m_channels(channels)
    , m_sampleRate(sampleRate)
{
    m_data.resize(numSamples * channels, 0.0f);
}

AudioBuffer::AudioBuffer(const AudioBuffer& other)
    : m_data(other.m_data)
    , m_numSamples(other.m_numSamples)
    , m_channels(other.m_channels)
    , m_sampleRate(other.m_sampleRate)
{
}

AudioBuffer::AudioBuffer(AudioBuffer&& other) noexcept
    : m_data(std::move(other.m_data))
    , m_numSamples(other.m_numSamples)
    , m_channels(other.m_channels)
    , m_sampleRate(other.m_sampleRate)
{
    other.m_numSamples = 0;
    other.m_channels = 0;
    other.m_sampleRate = 0;
}

AudioBuffer& AudioBuffer::operator=(const AudioBuffer& other) {
    if (this != &other) {
        m_data = other.m_data;
        m_numSamples = other.m_numSamples;
        m_channels = other.m_channels;
        m_sampleRate = other.m_sampleRate;
    }
    return *this;
}

AudioBuffer& AudioBuffer::operator=(AudioBuffer&& other) noexcept {
    if (this != &other) {
        m_data = std::move(other.m_data);
        m_numSamples = other.m_numSamples;
        m_channels = other.m_channels;
        m_sampleRate = other.m_sampleRate;
        other.m_numSamples = 0;
        other.m_channels = 0;
        other.m_sampleRate = 0;
    }
    return *this;
}

void AudioBuffer::allocate(size_t numSamples, int channels, int sampleRate) {
    m_numSamples = numSamples;
    m_channels = channels;
    m_sampleRate = sampleRate;
    m_data.resize(numSamples * channels, 0.0f);
}

void AudioBuffer::clear() {
    std::fill(m_data.begin(), m_data.end(), 0.0f);
}

void AudioBuffer::resize(size_t numSamples) {
    m_numSamples = numSamples;
    m_data.resize(numSamples * m_channels, 0.0f);
}

float* AudioBuffer::getChannelData(int channel) {
    if (channel < 0 || channel >= m_channels) {
        throw std::out_of_range("Channel index out of range");
    }
    // Assuming interleaved data
    return m_data.data() + channel;
}

const float* AudioBuffer::getChannelData(int channel) const {
    if (channel < 0 || channel >= m_channels) {
        throw std::out_of_range("Channel index out of range");
    }
    return m_data.data() + channel;
}

std::span<float> AudioBuffer::getChannel(int channel) {
    // For non-interleaved access, would need different layout
    return std::span<float>(m_data.data() + channel * m_numSamples, m_numSamples);
}

std::span<const float> AudioBuffer::getChannel(int channel) const {
    return std::span<const float>(m_data.data() + channel * m_numSamples, m_numSamples);
}

float AudioBuffer::getSample(int channel, size_t sample) const {
    return m_data[sample * m_channels + channel];
}

void AudioBuffer::setSample(int channel, size_t sample, float value) {
    m_data[sample * m_channels + channel] = value;
}

void AudioBuffer::setData(const float* data, size_t size) {
    size_t copySize = std::min(size, m_data.size());
    std::copy(data, data + copySize, m_data.begin());
}

void AudioBuffer::copyFrom(const AudioBuffer& other) {
    if (other.m_data.size() != m_data.size()) {
        m_data.resize(other.m_data.size());
    }
    m_numSamples = other.m_numSamples;
    m_channels = other.m_channels;
    m_sampleRate = other.m_sampleRate;
    std::copy(other.m_data.begin(), other.m_data.end(), m_data.begin());
}

void AudioBuffer::addFrom(const AudioBuffer& other, float gain) {
    if (other.m_data.size() != m_data.size()) return;

    SIMD::addWithGain(m_data.data(), other.m_data.data(), gain, m_data.size());
}

void AudioBuffer::applyGain(float gain) {
    SIMD::multiply(m_data.data(), gain, m_data.size());
}

void AudioBuffer::applyGainRamp(float startGain, float endGain) {
    float gainStep = (endGain - startGain) / static_cast<float>(m_numSamples);
    float currentGain = startGain;

    for (size_t i = 0; i < m_data.size(); ++i) {
        m_data[i] *= currentGain;
        if ((i + 1) % m_channels == 0) {
            currentGain += gainStep;
        }
    }
}

void AudioBuffer::normalize(float targetPeak) {
    float peak = getPeak();
    if (peak > 0.0f) {
        float gain = targetPeak / peak;
        applyGain(gain);
    }
}

float AudioBuffer::getPeak() const {
    float peak = 0.0f;
    for (float sample : m_data) {
        peak = std::max(peak, std::abs(sample));
    }
    return peak;
}

float AudioBuffer::getRMS() const {
    if (m_data.empty()) return 0.0f;

    float sumSquares = 0.0f;
    for (float sample : m_data) {
        sumSquares += sample * sample;
    }
    return std::sqrt(sumSquares / static_cast<float>(m_data.size()));
}

std::pair<float, float> AudioBuffer::getPeakLevels() const {
    if (m_channels < 2) {
        float peak = getPeak();
        return {peak, peak};
    }

    float leftPeak = 0.0f, rightPeak = 0.0f;

    for (size_t i = 0; i < m_numSamples; ++i) {
        leftPeak = std::max(leftPeak, std::abs(m_data[i * m_channels]));
        rightPeak = std::max(rightPeak, std::abs(m_data[i * m_channels + 1]));
    }

    return {leftPeak, rightPeak};
}

std::pair<float, float> AudioBuffer::getRMSLevels() const {
    if (m_channels < 2) {
        float rms = getRMS();
        return {rms, rms};
    }

    float leftSum = 0.0f, rightSum = 0.0f;

    for (size_t i = 0; i < m_numSamples; ++i) {
        float left = m_data[i * m_channels];
        float right = m_data[i * m_channels + 1];
        leftSum += left * left;
        rightSum += right * right;
    }

    float numSamples = static_cast<float>(m_numSamples);
    return {std::sqrt(leftSum / numSamples), std::sqrt(rightSum / numSamples)};
}

// AlignedAudioBuffer implementation
AlignedAudioBuffer::AlignedAudioBuffer(size_t numSamples, int channels)
    : m_size(numSamples * channels)
{
#ifdef _MSC_VER
    m_data = static_cast<float*>(_aligned_malloc(m_size * sizeof(float), ALIGNMENT));
#else
    m_data = static_cast<float*>(std::aligned_alloc(ALIGNMENT, m_size * sizeof(float)));
#endif
    if (!m_data) {
        throw std::bad_alloc();
    }
    std::fill(m_data, m_data + m_size, 0.0f);
}

AlignedAudioBuffer::~AlignedAudioBuffer() {
#ifdef _MSC_VER
    _aligned_free(m_data);
#else
    std::free(m_data);
#endif
}

} // namespace SoundBoost
