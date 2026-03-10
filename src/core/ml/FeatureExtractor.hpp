// src/core/ml/FeatureExtractor.hpp
#pragma once

#include <vector>
#include <memory>

namespace SoundBoost {

class AudioBuffer;
class FFTProcessor;

class FeatureExtractor {
public:
    FeatureExtractor();
    ~FeatureExtractor();

    void initialize(int sampleRate, int fftSize, int hopSize);

    // Feature extraction methods
    std::vector<float> extractMFCC(const AudioBuffer& buffer, int numCoeffs);
    std::vector<float> extractSpectralFeatures(const AudioBuffer& buffer);
    std::vector<float> extractTemporalFeatures(const AudioBuffer& buffer);
    std::vector<float> extractChromaFeatures(const AudioBuffer& buffer);

    // Individual features
    float calculateSpectralCentroid(const std::vector<float>& spectrum);
    float calculateSpectralRolloff(const std::vector<float>& spectrum, float percentage = 0.85f);
    float calculateSpectralFlux(const std::vector<float>& spectrum);
    float calculateSpectralFlatness(const std::vector<float>& spectrum);
    float calculateZeroCrossingRate(const AudioBuffer& buffer);

    // Audio characteristics
    float calculateBrightness();
    float calculateWarmth();
    float calculateDynamicRange();
    float calculateBassContent();

private:
    void computeMelFilterbank();
    void computeDCT();
    std::vector<float> applyMelFilterbank(const std::vector<float>& spectrum);

    int m_sampleRate{48000};
    int m_fftSize{2048};
    int m_hopSize{512};
    int m_numMelBands{40};

    std::unique_ptr<FFTProcessor> m_fftProcessor;

    std::vector<std::vector<float>> m_melFilterbank;
    std::vector<std::vector<float>> m_dctMatrix;

    // State for flux calculation
    std::vector<float> m_previousSpectrum;

    // Cached features for characteristic calculation
    std::vector<float> m_lastSpectrum;
    float m_lastRMS{0.0f};
    float m_peakLevel{0.0f};
};

} // namespace SoundBoost
