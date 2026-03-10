// src/core/dsp/DSPProcessor.hpp
#pragma once

#include "BiquadFilter.hpp"
#include "FFTProcessor.hpp"
#include "../audio/AudioBuffer.hpp"

#include <vector>
#include <memory>
#include <complex>

namespace SoundBoost {

class DSPProcessor {
public:
    DSPProcessor();
    ~DSPProcessor();

    void initialize(int sampleRate, int bufferSize);
    void reset();

    // Filter operations
    void applyLowPass(AudioBuffer& buffer, float cutoffHz, float Q = 0.707f);
    void applyHighPass(AudioBuffer& buffer, float cutoffHz, float Q = 0.707f);
    void applyBandPass(AudioBuffer& buffer, float centerHz, float bandwidth);
    void applyNotch(AudioBuffer& buffer, float centerHz, float Q = 1.0f);
    void applyPeakEQ(AudioBuffer& buffer, float centerHz, float gainDb, float Q);
    void applyLowShelf(AudioBuffer& buffer, float cutoffHz, float gainDb, float slope = 1.0f);
    void applyHighShelf(AudioBuffer& buffer, float cutoffHz, float gainDb, float slope = 1.0f);

    // FFT operations
    void computeFFT(const AudioBuffer& input, std::vector<std::complex<float>>& output);
    void computeIFFT(const std::vector<std::complex<float>>& input, AudioBuffer& output);
    std::vector<float> computeMagnitudeSpectrum(const AudioBuffer& buffer);
    std::vector<float> computePhaseSpectrum(const AudioBuffer& buffer);

    // Convolution
    void convolve(AudioBuffer& buffer, const std::vector<float>& impulseResponse);

    // Dynamics
    void applyCompression(AudioBuffer& buffer, float threshold, float ratio,
                         float attack, float release);
    void applyLimiting(AudioBuffer& buffer, float ceiling);
    void applyGate(AudioBuffer& buffer, float threshold, float attack, float release);

    // Utilities
    void normalize(AudioBuffer& buffer, float targetLevel = 1.0f);
    void dcOffset(AudioBuffer& buffer);
    void fadeIn(AudioBuffer& buffer, float durationMs);
    void fadeOut(AudioBuffer& buffer, float durationMs);

    // State
    int getSampleRate() const { return m_sampleRate; }
    int getBufferSize() const { return m_bufferSize; }

private:
    int m_sampleRate{48000};
    int m_bufferSize{512};

    std::unique_ptr<FFTProcessor> m_fftProcessor;
    std::vector<std::unique_ptr<BiquadFilter>> m_filterBank;

    // Compressor state
    float m_compEnvelope{0.0f};

    // DC offset filter state
    float m_dcX1{0.0f}, m_dcY1{0.0f};
};

} // namespace SoundBoost
