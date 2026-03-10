// src/core/dsp/FFTProcessor.cpp
#include "FFTProcessor.hpp"
#include "../audio/AudioBuffer.hpp"

#include <cmath>
#include <algorithm>
#include <numbers>

namespace SoundBoost {

FFTProcessor::FFTProcessor() = default;
FFTProcessor::~FFTProcessor() = default;

void FFTProcessor::initialize(int fftSize) {
    // Ensure power of 2
    int size = 1;
    while (size < fftSize) size *= 2;
    m_fftSize = size;

    // Allocate buffers
    m_twiddleFactors.resize(m_fftSize);
    m_window.resize(m_fftSize);
    m_workBuffer.resize(m_fftSize);
    m_realBuffer.resize(m_fftSize);
    m_bitReversalTable.resize(m_fftSize);

    computeTwiddleFactors();
    computeWindow();

    // Compute bit reversal table
    int bits = 0;
    for (int i = m_fftSize; i > 1; i >>= 1) ++bits;

    for (int i = 0; i < m_fftSize; ++i) {
        int reversed = 0;
        for (int j = 0; j < bits; ++j) {
            if (i & (1 << j)) {
                reversed |= (1 << (bits - 1 - j));
            }
        }
        m_bitReversalTable[i] = reversed;
    }

    m_initialized = true;
}

void FFTProcessor::computeTwiddleFactors() {
    const float twoPi = 2.0f * std::numbers::pi_v<float>;

    for (int i = 0; i < m_fftSize; ++i) {
        float angle = -twoPi * i / m_fftSize;
        m_twiddleFactors[i] = std::complex<float>(std::cos(angle), std::sin(angle));
    }
}

void FFTProcessor::computeWindow() {
    const float pi = std::numbers::pi_v<float>;
    const float twoPi = 2.0f * pi;

    switch (m_windowType) {
        case WindowType::None:
            std::fill(m_window.begin(), m_window.end(), 1.0f);
            break;
        case WindowType::Hann:
            for (int i = 0; i < m_fftSize; ++i) {
                m_window[i] = 0.5f * (1.0f - std::cos(twoPi * i / (m_fftSize - 1)));
            }
            break;
        case WindowType::Hamming:
            for (int i = 0; i < m_fftSize; ++i) {
                m_window[i] = 0.54f - 0.46f * std::cos(twoPi * i / (m_fftSize - 1));
            }
            break;
        case WindowType::Blackman:
            for (int i = 0; i < m_fftSize; ++i) {
                float a0 = 0.42f, a1 = 0.5f, a2 = 0.08f;
                m_window[i] = a0 - a1 * std::cos(twoPi * i / (m_fftSize - 1))
                            + a2 * std::cos(4.0f * pi * i / (m_fftSize - 1));
            }
            break;
        default:
            std::fill(m_window.begin(), m_window.end(), 1.0f);
    }
}

void FFTProcessor::setWindow(WindowType type) {
    m_windowType = type;
    if (m_initialized) computeWindow();
}

void FFTProcessor::applyWindow(float* data, int size) {
    int windowSize = std::min(size, m_fftSize);
    for (int i = 0; i < windowSize; ++i) {
        data[i] *= m_window[i];
    }
}

void FFTProcessor::forward(const float* input, std::complex<float>* output) {
    if (!m_initialized) return;
    for (int i = 0; i < m_fftSize; ++i) {
        m_workBuffer[i] = std::complex<float>(input[i] * m_window[i], 0.0f);
    }
    fftIterative(m_workBuffer.data(), m_fftSize, false);
    std::copy(m_workBuffer.begin(), m_workBuffer.end(), output);
}

void FFTProcessor::forward(const AudioBuffer& input, std::vector<std::complex<float>>& output) {
    if (!m_initialized) return;
    const float* data = input.getData();
    size_t numSamples = input.getNumSamples();
    int channels = input.getNumChannels();

    for (int i = 0; i < m_fftSize; ++i) {
        if (i < static_cast<int>(numSamples)) {
            float sample = 0.0f;
            for (int ch = 0; ch < channels; ++ch) {
                sample += data[i * channels + ch];
            }
            m_realBuffer[i] = sample / channels;
        } else {
            m_realBuffer[i] = 0.0f;
        }
    }
    output.resize(m_fftSize);
    forward(m_realBuffer.data(), output.data());
}

void FFTProcessor::inverse(const std::complex<float>* input, float* output) {
    if (!m_initialized) return;
    std::copy(input, input + m_fftSize, m_workBuffer.begin());
    fftIterative(m_workBuffer.data(), m_fftSize, true);
    float scale = 1.0f / m_fftSize;
    for (int i = 0; i < m_fftSize; ++i) {
        output[i] = m_workBuffer[i].real() * scale;
    }
}

void FFTProcessor::fftIterative(std::complex<float>* data, int n, bool inverse) {
    // Bit reversal
    for (int i = 0; i < n; ++i) {
        int j = m_bitReversalTable[i];
        if (i < j) std::swap(data[i], data[j]);
    }

    // Cooley-Tukey
    for (int len = 2; len <= n; len *= 2) {
        float angle = (inverse ? 2.0f : -2.0f) * std::numbers::pi_v<float> / len;
        std::complex<float> wLen(std::cos(angle), std::sin(angle));

        for (int i = 0; i < n; i += len) {
            std::complex<float> w(1.0f, 0.0f);
            for (int j = 0; j < len / 2; ++j) {
                std::complex<float> u = data[i + j];
                std::complex<float> t = w * data[i + j + len / 2];
                data[i + j] = u + t;
                data[i + j + len / 2] = u - t;
                w *= wLen;
            }
        }
    }
}

std::vector<float> FFTProcessor::computeMagnitudeSpectrum(const AudioBuffer& buffer) {
    std::vector<std::complex<float>> spectrum;
    forward(buffer, spectrum);
    int numBins = getNumBins();
    std::vector<float> magnitudes(numBins);
    for (int i = 0; i < numBins; ++i) {
        magnitudes[i] = std::abs(spectrum[i]);
    }
    return magnitudes;
}

std::vector<float> FFTProcessor::computePowerSpectrum(const AudioBuffer& buffer) {
    auto spectrum = computeMagnitudeSpectrum(buffer);
    for (float& v : spectrum) v *= v;
    return spectrum;
}

std::vector<float> FFTProcessor::computePhaseSpectrum(const AudioBuffer& buffer) {
    std::vector<std::complex<float>> spectrum;
    forward(buffer, spectrum);
    int numBins = getNumBins();
    std::vector<float> phase(numBins);
    for (int i = 0; i < numBins; ++i) {
        phase[i] = std::arg(spectrum[i]);
    }
    return phase;
}

void FFTProcessor::reset() {
    std::fill(m_workBuffer.begin(), m_workBuffer.end(), std::complex<float>(0, 0));
    std::fill(m_realBuffer.begin(), m_realBuffer.end(), 0.0f);
}

} // namespace SoundBoost
