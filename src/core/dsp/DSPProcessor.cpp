// src/core/dsp/DSPProcessor.cpp
#include "DSPProcessor.hpp"
#include "FilterDesign.hpp"
#include "SIMD/SIMDDispatcher.hpp"
#include "../../utils/Logger.hpp"

#include <cmath>
#include <algorithm>

namespace SoundBoost {

DSPProcessor::DSPProcessor() {
    m_fftProcessor = std::make_unique<FFTProcessor>();
}

DSPProcessor::~DSPProcessor() = default;

void DSPProcessor::initialize(int sampleRate, int bufferSize) {
    m_sampleRate = sampleRate;
    m_bufferSize = bufferSize;

    m_fftProcessor->initialize(bufferSize * 2); // 2x for zero-padding

    // Pre-allocate filter bank
    m_filterBank.reserve(32);

    LOG_DEBUG("DSPProcessor initialized: {}Hz, {} samples", sampleRate, bufferSize);
}

void DSPProcessor::reset() {
    m_compEnvelope = 0.0f;
    m_dcX1 = m_dcY1 = 0.0f;

    for (auto& filter : m_filterBank) {
        filter->reset();
    }
}

void DSPProcessor::applyLowPass(AudioBuffer& buffer, float cutoffHz, float Q) {
    auto coeffs = FilterDesign::lowPass(m_sampleRate, cutoffHz, Q);

    BiquadFilter filter;
    filter.setCoefficients(coeffs);

    float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    for (int ch = 0; ch < channels; ++ch) {
        filter.reset();
        for (size_t i = 0; i < numSamples; ++i) {
            size_t idx = i * channels + ch;
            data[idx] = filter.process(data[idx]);
        }
    }
}

void DSPProcessor::applyHighPass(AudioBuffer& buffer, float cutoffHz, float Q) {
    auto coeffs = FilterDesign::highPass(m_sampleRate, cutoffHz, Q);

    BiquadFilter filter;
    filter.setCoefficients(coeffs);

    float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    for (int ch = 0; ch < channels; ++ch) {
        filter.reset();
        for (size_t i = 0; i < numSamples; ++i) {
            size_t idx = i * channels + ch;
            data[idx] = filter.process(data[idx]);
        }
    }
}

void DSPProcessor::applyBandPass(AudioBuffer& buffer, float centerHz, float bandwidth) {
    auto coeffs = FilterDesign::bandPass(m_sampleRate, centerHz, bandwidth);

    BiquadFilter filter;
    filter.setCoefficients(coeffs);

    float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    for (int ch = 0; ch < channels; ++ch) {
        filter.reset();
        for (size_t i = 0; i < numSamples; ++i) {
            size_t idx = i * channels + ch;
            data[idx] = filter.process(data[idx]);
        }
    }
}

void DSPProcessor::applyNotch(AudioBuffer& buffer, float centerHz, float Q) {
    auto coeffs = FilterDesign::notch(m_sampleRate, centerHz, Q);

    BiquadFilter filter;
    filter.setCoefficients(coeffs);

    float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    for (int ch = 0; ch < channels; ++ch) {
        filter.reset();
        for (size_t i = 0; i < numSamples; ++i) {
            size_t idx = i * channels + ch;
            data[idx] = filter.process(data[idx]);
        }
    }
}

void DSPProcessor::applyPeakEQ(AudioBuffer& buffer, float centerHz, float gainDb, float Q) {
    auto coeffs = FilterDesign::peakEQ(m_sampleRate, centerHz, gainDb, Q);

    BiquadFilter filter;
    filter.setCoefficients(coeffs);

    float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    for (int ch = 0; ch < channels; ++ch) {
        filter.reset();
        for (size_t i = 0; i < numSamples; ++i) {
            size_t idx = i * channels + ch;
            data[idx] = filter.process(data[idx]);
        }
    }
}

void DSPProcessor::applyLowShelf(AudioBuffer& buffer, float cutoffHz, float gainDb, float slope) {
    auto coeffs = FilterDesign::lowShelf(m_sampleRate, cutoffHz, gainDb, slope);

    BiquadFilter filter;
    filter.setCoefficients(coeffs);

    float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    for (int ch = 0; ch < channels; ++ch) {
        filter.reset();
        for (size_t i = 0; i < numSamples; ++i) {
            size_t idx = i * channels + ch;
            data[idx] = filter.process(data[idx]);
        }
    }
}

void DSPProcessor::applyHighShelf(AudioBuffer& buffer, float cutoffHz, float gainDb, float slope) {
    auto coeffs = FilterDesign::highShelf(m_sampleRate, cutoffHz, gainDb, slope);

    BiquadFilter filter;
    filter.setCoefficients(coeffs);

    float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    for (int ch = 0; ch < channels; ++ch) {
        filter.reset();
        for (size_t i = 0; i < numSamples; ++i) {
            size_t idx = i * channels + ch;
            data[idx] = filter.process(data[idx]);
        }
    }
}

std::vector<float> DSPProcessor::computeMagnitudeSpectrum(const AudioBuffer& buffer) {
    std::vector<std::complex<float>> spectrum;
    computeFFT(buffer, spectrum);

    std::vector<float> magnitudes(spectrum.size() / 2);
    for (size_t i = 0; i < magnitudes.size(); ++i) {
        magnitudes[i] = std::abs(spectrum[i]);
    }

    return magnitudes;
}

void DSPProcessor::applyCompression(AudioBuffer& buffer, float threshold,
                                    float ratio, float attack, float release) {
    float thresholdLinear = std::pow(10.0f, threshold / 20.0f);
    float attackCoeff = std::exp(-1.0f / (attack * m_sampleRate / 1000.0f));
    float releaseCoeff = std::exp(-1.0f / (release * m_sampleRate / 1000.0f));

    float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    for (size_t i = 0; i < numSamples; ++i) {
        // Get max level across channels for this sample
        float maxLevel = 0.0f;
        for (int ch = 0; ch < channels; ++ch) {
            maxLevel = std::max(maxLevel, std::abs(data[i * channels + ch]));
        }

        // Envelope follower
        float coeff = (maxLevel > m_compEnvelope) ? attackCoeff : releaseCoeff;
        m_compEnvelope = coeff * m_compEnvelope + (1.0f - coeff) * maxLevel;

        // Calculate gain reduction
        float gainReduction = 1.0f;
        if (m_compEnvelope > thresholdLinear) {
            float dbOver = 20.0f * std::log10(m_compEnvelope / thresholdLinear);
            float dbReduction = dbOver * (1.0f - 1.0f / ratio);
            gainReduction = std::pow(10.0f, -dbReduction / 20.0f);
        }

        // Apply gain reduction to all channels
        for (int ch = 0; ch < channels; ++ch) {
            data[i * channels + ch] *= gainReduction;
        }
    }
}

void DSPProcessor::applyLimiting(AudioBuffer& buffer, float ceiling) {
    float ceilingLinear = std::pow(10.0f, ceiling / 20.0f);

    float* data = buffer.getData();
    size_t size = buffer.getSize();

    SIMD::clip(data, -ceilingLinear, ceilingLinear, size);
}

void DSPProcessor::applyGate(AudioBuffer& buffer, float threshold,
                            float attack, float release) {
    float thresholdLinear = std::pow(10.0f, threshold / 20.0f);
    float attackCoeff = std::exp(-1.0f / (attack * m_sampleRate / 1000.0f));
    float releaseCoeff = std::exp(-1.0f / (release * m_sampleRate / 1000.0f));

    float gateGain = 0.0f;

    float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    for (size_t i = 0; i < numSamples; ++i) {
        float maxLevel = 0.0f;
        for (int ch = 0; ch < channels; ++ch) {
            maxLevel = std::max(maxLevel, std::abs(data[i * channels + ch]));
        }

        float targetGain = (maxLevel > thresholdLinear) ? 1.0f : 0.0f;
        float coeff = (targetGain > gateGain) ? attackCoeff : releaseCoeff;
        gateGain = coeff * gateGain + (1.0f - coeff) * targetGain;

        for (int ch = 0; ch < channels; ++ch) {
            data[i * channels + ch] *= gateGain;
        }
    }
}

void DSPProcessor::dcOffset(AudioBuffer& buffer) {
    // DC blocking filter: y[n] = x[n] - x[n-1] + 0.995 * y[n-1]
    const float R = 0.995f;

    float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    for (size_t i = 0; i < numSamples; ++i) {
        for (int ch = 0; ch < channels; ++ch) {
            float x = data[i * channels + ch];
            float y = x - m_dcX1 + R * m_dcY1;
            m_dcX1 = x;
            m_dcY1 = y;
            data[i * channels + ch] = y;
        }
    }
}

void DSPProcessor::fadeIn(AudioBuffer& buffer, float durationMs) {
    int fadeSamples = static_cast<int>((durationMs / 1000.0f) * m_sampleRate);
    fadeSamples = std::min(fadeSamples, static_cast<int>(buffer.getNumSamples()));

    float* data = buffer.getData();
    int channels = buffer.getNumChannels();

    for (int i = 0; i < fadeSamples; ++i) {
        float gain = static_cast<float>(i) / fadeSamples;
        // Apply smooth fade curve
        gain = 0.5f * (1.0f - std::cos(gain * M_PI));

        for (int ch = 0; ch < channels; ++ch) {
            data[i * channels + ch] *= gain;
        }
    }
}

void DSPProcessor::fadeOut(AudioBuffer& buffer, float durationMs) {
    int fadeSamples = static_cast<int>((durationMs / 1000.0f) * m_sampleRate);
    size_t numSamples = buffer.getNumSamples();
    int startSample = std::max(0, static_cast<int>(numSamples) - fadeSamples);

    float* data = buffer.getData();
    int channels = buffer.getNumChannels();

    for (size_t i = startSample; i < numSamples; ++i) {
        float gain = 1.0f - static_cast<float>(i - startSample) / fadeSamples;
        gain = 0.5f * (1.0f - std::cos(gain * M_PI));

        for (int ch = 0; ch < channels; ++ch) {
            data[i * channels + ch] *= gain;
        }
    }
}

} // namespace SoundBoost
