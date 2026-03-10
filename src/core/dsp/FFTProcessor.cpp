// src/core/dsp/FFTProcessor.cpp
#include "FFTProcessor.hpp"
#include <cmath>
#include <algorithm>

namespace SoundBoost {

void FFTProcessor::initialize(size_t fftSize) {
    m_fftSize = fftSize;
    m_workBuffer.resize(fftSize);

    // Build bit reversal table
    m_bitReversalTable.resize(fftSize);
    size_t bits = 0;
    while ((1u << bits) < fftSize) bits++;

    for (size_t i = 0; i < fftSize; ++i) {
        size_t rev = 0;
        for (size_t j = 0; j < bits; ++j) {
            if (i & (1u << j)) {
                rev |= (1u << (bits - 1 - j));
            }
        }
        m_bitReversalTable[i] = rev;
    }

    // Build twiddle factors
    m_twiddleFactors.resize(fftSize / 2);
    for (size_t i = 0; i < fftSize / 2; ++i) {
        float angle = -2.0f * M_PI * i / fftSize;
        m_twiddleFactors[i] = std::complex<float>(std::cos(angle), std::sin(angle));
    }
}

void FFTProcessor::fft(std::complex<float>* data, bool inverse) {
    const size_t n = m_fftSize;

    // Bit reversal
    for (size_t i = 0; i < n; ++i) {
        size_t j = m_bitReversalTable[i];
        if (i < j) {
            std::swap(data[i], data[j]);
        }
    }

    // Cooley-Tukey FFT
    for (size_t len = 2; len <= n; len *= 2) {
        size_t halfLen = len / 2;
        float angle = (inverse ? 2.0f : -2.0f) * M_PI / len;
        std::complex<float> wlen(std::cos(angle), std::sin(angle));

        for (size_t i = 0; i < n; i += len) {
            std::complex<float> w(1.0f, 0.0f);
            for (size_t j = 0; j < halfLen; ++j) {
                std::complex<float> u = data[i + j];
                std::complex<float> v = data[i + j + halfLen] * w;
                data[i + j] = u + v;
                data[i + j + halfLen] = u - v;
                w *= wlen;
            }
        }
    }

    // Normalize for inverse FFT
    if (inverse) {
        for (size_t i = 0; i < n; ++i) {
            data[i] /= n;
        }
    }
}

void FFTProcessor::rfft(const float* input, std::complex<float>* output) {
    // Pack real input into complex
    for (size_t i = 0; i < m_fftSize; ++i) {
        m_workBuffer[i] = std::complex<float>(input[i], 0.0f);
    }

    // Perform complex FFT
    fft(m_workBuffer.data(), false);

    // Extract positive frequencies (0 to Nyquist)
    size_t numBins = m_fftSize / 2 + 1;
    for (size_t i = 0; i < numBins; ++i) {
        output[i] = m_workBuffer[i];
    }
}

void FFTProcessor::rifft(const std::complex<float>* input, float* output) {
    // Pack into full spectrum (exploit conjugate symmetry)
    size_t numBins = m_fftSize / 2 + 1;

    m_workBuffer[0] = input[0];
    for (size_t i = 1; i < numBins - 1; ++i) {
        m_workBuffer[i] = input[i];
        m_workBuffer[m_fftSize - i] = std::conj(input[i]);
    }
    if (m_fftSize % 2 == 0) {
        m_workBuffer[numBins - 1] = input[numBins - 1];
    }

    // Perform inverse FFT
    fft(m_workBuffer.data(), true);

    // Extract real part
    for (size_t i = 0; i < m_fftSize; ++i) {
        output[i] = m_workBuffer[i].real();
    }
}

void FFTProcessor::forward(const float* input, std::complex<float>* output) {
    rfft(input, output);
}

void FFTProcessor::forward(const AudioBuffer& input, std::vector<std::complex<float>>& output) {
    size_t numBins = m_fftSize / 2 + 1;
    output.resize(numBins);

    // Use first channel
    std::vector<float> temp(m_fftSize, 0.0f);
    size_t copySize = std::min(m_fftSize, input.getNumSamples());
    std::copy(input.getData(), input.getData() + copySize, temp.begin());

    rfft(temp.data(), output.data());
}

void FFTProcessor::inverse(const std::complex<float>* input, float* output) {
    rifft(input, output);
}

void FFTProcessor::inverse(const std::vector<std::complex<float>>& input, AudioBuffer& output) {
    std::vector<float> temp(m_fftSize);
    rifft(input.data(), temp.data());

    // Copy to output
    size_t copySize = std::min(m_fftSize, output.getNumSamples());
    std::copy(temp.begin(), temp.begin() + copySize, output.getData());
}

std::vector<float> FFTProcessor::computeMagnitudeSpectrum(const AudioBuffer& buffer) {
    std::vector<std::complex<float>> spectrum;
    forward(buffer, spectrum);

    size_t numBins = spectrum.size();
    std::vector<float> magnitudes(numBins);

    for (size_t i = 0; i < numBins; ++i) {
        magnitudes[i] = std::abs(spectrum[i]);
    }

    return magnitudes;
}

std::vector<float> FFTProcessor::computePhaseSpectrum(const AudioBuffer& buffer) {
    std::vector<std::complex<float>> spectrum;
    forward(buffer, spectrum);

    size_t numBins = spectrum.size();
    std::vector<float> phases(numBins);

    for (size_t i = 0; i < numBins; ++i) {
        phases[i] = std::arg(spectrum[i]);
    }

    return phases;
}

} // namespace SoundBoost
