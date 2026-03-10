// src/core/dsp/FFTProcessor.hpp
#pragma once

#include <vector>
#include <complex>
#include "../audio/AudioBuffer.hpp"

namespace SoundBoost {

class FFTProcessor {
public:
    FFTProcessor() = default;
    ~FFTProcessor() = default;

    void initialize(size_t fftSize);

    // Forward FFT
    void forward(const float* input, std::complex<float>* output);
    void forward(const AudioBuffer& input, std::vector<std::complex<float>>& output);

    // Inverse FFT
    void inverse(const std::complex<float>* input, float* output);
    void inverse(const std::vector<std::complex<float>>& input, AudioBuffer& output);

    // Magnitude and phase
    std::vector<float> computeMagnitudeSpectrum(const AudioBuffer& buffer);
    std::vector<float> computePhaseSpectrum(const AudioBuffer& buffer);

    // Properties
    size_t getFFTSize() const { return m_fftSize; }
    size_t getNumBins() const { return m_fftSize / 2 + 1; }

private:
    void fft(std::complex<float>* data, bool inverse);
    void rfft(const float* input, std::complex<float>* output);
    void rifft(const std::complex<float>* input, float* output);

    size_t m_fftSize{1024};
    std::vector<std::complex<float>> m_workBuffer;
    std::vector<int> m_bitReversalTable;
    std::vector<std::complex<float>> m_twiddleFactors;
};

} // namespace SoundBoost
