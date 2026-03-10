// src/core/ml/FeatureExtractor.cpp
#include "FeatureExtractor.hpp"
#include "../audio/AudioBuffer.hpp"
#include "../dsp/FFTProcessor.hpp"

#include <cmath>
#include <algorithm>
#include <numeric>

namespace SoundBoost {

FeatureExtractor::FeatureExtractor()
    : m_fftProcessor(std::make_unique<FFTProcessor>())
{
}

FeatureExtractor::~FeatureExtractor() = default;

void FeatureExtractor::initialize(int sampleRate, int fftSize, int hopSize) {
    m_sampleRate = sampleRate;
    m_fftSize = fftSize;
    m_hopSize = hopSize;

    m_fftProcessor->initialize(fftSize);

    computeMelFilterbank();
    computeDCT();

    m_previousSpectrum.resize(fftSize / 2 + 1, 0.0f);
}

void FeatureExtractor::computeMelFilterbank() {
    auto hzToMel = [](float hz) {
        return 2595.0f * std::log10(1.0f + hz / 700.0f);
    };

    auto melToHz = [](float mel) {
        return 700.0f * (std::pow(10.0f, mel / 2595.0f) - 1.0f);
    };

    float lowFreq = 0.0f;
    float highFreq = m_sampleRate / 2.0f;

    float lowMel = hzToMel(lowFreq);
    float highMel = hzToMel(highFreq);

    std::vector<float> melPoints(m_numMelBands + 2);
    for (int i = 0; i <= m_numMelBands + 1; ++i) {
        melPoints[i] = lowMel + (highMel - lowMel) * i / (m_numMelBands + 1);
    }

    std::vector<int> binPoints(m_numMelBands + 2);
    int numBins = m_fftSize / 2 + 1;
    for (int i = 0; i <= m_numMelBands + 1; ++i) {
        float hz = melToHz(melPoints[i]);
        binPoints[i] = static_cast<int>(std::floor((m_fftSize + 1) * hz / m_sampleRate));
    }

    m_melFilterbank.resize(m_numMelBands);
    for (int i = 0; i < m_numMelBands; ++i) {
        m_melFilterbank[i].resize(numBins, 0.0f);

        for (int j = binPoints[i]; j < binPoints[i + 1]; ++j) {
            m_melFilterbank[i][j] = static_cast<float>(j - binPoints[i]) /
                                    (binPoints[i + 1] - binPoints[i]);
        }
        for (int j = binPoints[i + 1]; j < binPoints[i + 2]; ++j) {
            m_melFilterbank[i][j] = static_cast<float>(binPoints[i + 2] - j) /
                                    (binPoints[i + 2] - binPoints[i + 1]);
        }
    }
}

void FeatureExtractor::computeDCT() {
    m_dctMatrix.resize(13);
    for (int i = 0; i < 13; ++i) {
        m_dctMatrix[i].resize(m_numMelBands);
        for (int j = 0; j < m_numMelBands; ++j) {
            m_dctMatrix[i][j] = std::cos(M_PI * i * (j + 0.5f) / m_numMelBands);
        }
    }
}

std::vector<float> FeatureExtractor::applyMelFilterbank(const std::vector<float>& spectrum) {
    std::vector<float> melEnergies(m_numMelBands, 0.0f);

    for (int i = 0; i < m_numMelBands; ++i) {
        for (size_t j = 0; j < spectrum.size() && j < m_melFilterbank[i].size(); ++j) {
            melEnergies[i] += spectrum[j] * m_melFilterbank[i][j];
        }
        melEnergies[i] = std::log(std::max(melEnergies[i], 1e-10f));
    }

    return melEnergies;
}

std::vector<float> FeatureExtractor::extractMFCC(const AudioBuffer& buffer, int numCoeffs) {
    numCoeffs = std::min(numCoeffs, 13);

    auto spectrum = m_fftProcessor->computeMagnitudeSpectrum(buffer);
    auto melEnergies = applyMelFilterbank(spectrum);

    std::vector<float> mfcc(numCoeffs, 0.0f);
    for (int i = 0; i < numCoeffs; ++i) {
        for (int j = 0; j < m_numMelBands; ++j) {
            mfcc[i] += m_dctMatrix[i][j] * melEnergies[j];
        }
    }

    return mfcc;
}

std::vector<float> FeatureExtractor::extractSpectralFeatures(const AudioBuffer& buffer) {
    auto spectrum = m_fftProcessor->computeMagnitudeSpectrum(buffer);
    m_lastSpectrum = spectrum;

    std::vector<float> features;
    features.push_back(calculateSpectralCentroid(spectrum));
    features.push_back(calculateSpectralRolloff(spectrum, 0.85f));
    features.push_back(calculateSpectralRolloff(spectrum, 0.95f));
    features.push_back(calculateSpectralFlux(spectrum));
    features.push_back(calculateSpectralFlatness(spectrum));
    features.push_back(calculateZeroCrossingRate(buffer));

    m_previousSpectrum = spectrum;
    return features;
}

std::vector<float> FeatureExtractor::extractTemporalFeatures(const AudioBuffer& buffer) {
    std::vector<float> features;

    float rms = buffer.getRMS();
    m_lastRMS = rms;
    features.push_back(rms);

    float peak = buffer.getPeak();
    m_peakLevel = std::max(m_peakLevel * 0.99f, peak);
    features.push_back(peak);

    features.push_back(rms > 0 ? peak / rms : 0.0f);
    features.push_back(calculateZeroCrossingRate(buffer));

    return features;
}

std::vector<float> FeatureExtractor::extractChromaFeatures(const AudioBuffer& buffer) {
    auto spectrum = m_fftProcessor->computeMagnitudeSpectrum(buffer);

    std::vector<float> chroma(12, 0.0f);
    int numBins = spectrum.size();
    float binWidth = static_cast<float>(m_sampleRate) / m_fftSize;

    for (int i = 1; i < numBins; ++i) {
        float freq = i * binWidth;
        if (freq < 20.0f || freq > 5000.0f) continue;

        float midiNote = 12.0f * std::log2(freq / 440.0f) + 69.0f;
        int chromaIndex = static_cast<int>(std::round(midiNote)) % 12;
        if (chromaIndex < 0) chromaIndex += 12;

        chroma[chromaIndex] += spectrum[i] * spectrum[i];
    }

    float maxChroma = *std::max_element(chroma.begin(), chroma.end());
    if (maxChroma > 0) {
        for (float& c : chroma) {
            c /= maxChroma;
        }
    }

    return chroma;
}

float FeatureExtractor::calculateSpectralCentroid(const std::vector<float>& spectrum) {
    float weightedSum = 0.0f;
    float totalEnergy = 0.0f;
    float binWidth = static_cast<float>(m_sampleRate) / m_fftSize;

    for (size_t i = 0; i < spectrum.size(); ++i) {
        float freq = i * binWidth;
        weightedSum += freq * spectrum[i];
        totalEnergy += spectrum[i];
    }

    return totalEnergy > 0 ? weightedSum / totalEnergy : 0.0f;
}

float FeatureExtractor::calculateSpectralRolloff(const std::vector<float>& spectrum, float percentage) {
    float totalEnergy = std::accumulate(spectrum.begin(), spectrum.end(), 0.0f);
    float threshold = totalEnergy * percentage;

    float cumSum = 0.0f;
    float binWidth = static_cast<float>(m_sampleRate) / m_fftSize;

    for (size_t i = 0; i < spectrum.size(); ++i) {
        cumSum += spectrum[i];
        if (cumSum >= threshold) {
            return i * binWidth;
        }
    }

    return m_sampleRate / 2.0f;
}

float FeatureExtractor::calculateSpectralFlux(const std::vector<float>& spectrum) {
    if (m_previousSpectrum.empty() || m_previousSpectrum.size() != spectrum.size()) {
        return 0.0f;
    }

    float flux = 0.0f;
    for (size_t i = 0; i < spectrum.size(); ++i) {
        float diff = spectrum[i] - m_previousSpectrum[i];
        flux += diff * diff;
    }

    return std::sqrt(flux);
}

float FeatureExtractor::calculateSpectralFlatness(const std::vector<float>& spectrum) {
    if (spectrum.empty()) return 0.0f;

    float geometricMean = 0.0f;
    float arithmeticMean = 0.0f;
    int count = 0;

    for (float s : spectrum) {
        if (s > 1e-10f) {
            geometricMean += std::log(s);
            arithmeticMean += s;
            ++count;
        }
    }

    if (count == 0 || arithmeticMean == 0) return 0.0f;

    geometricMean = std::exp(geometricMean / count);
    arithmeticMean /= count;

    return geometricMean / arithmeticMean;
}

float FeatureExtractor::calculateZeroCrossingRate(const AudioBuffer& buffer) {
    const float* data = buffer.getData();
    size_t numSamples = buffer.getNumSamples();
    int channels = buffer.getNumChannels();

    int crossings = 0;
    for (size_t i = 1; i < numSamples; ++i) {
        float prev = data[(i - 1) * channels];
        float curr = data[i * channels];
        if ((prev >= 0 && curr < 0) || (prev < 0 && curr >= 0)) {
            ++crossings;
        }
    }

    return static_cast<float>(crossings) / numSamples;
}

float FeatureExtractor::calculateBrightness() {
    if (m_lastSpectrum.empty()) return 0.5f;
    return std::clamp(calculateSpectralCentroid(m_lastSpectrum) / 10000.0f, 0.0f, 1.0f);
}

float FeatureExtractor::calculateWarmth() {
    if (m_lastSpectrum.empty()) return 0.5f;

    float binWidth = static_cast<float>(m_sampleRate) / m_fftSize;
    int lowCutoffBin = static_cast<int>(500.0f / binWidth);

    float lowEnergy = 0.0f;
    float totalEnergy = 0.0f;

    for (size_t i = 0; i < m_lastSpectrum.size(); ++i) {
        totalEnergy += m_lastSpectrum[i];
        if (static_cast<int>(i) < lowCutoffBin) {
            lowEnergy += m_lastSpectrum[i];
        }
    }

    return totalEnergy > 0 ? lowEnergy / totalEnergy : 0.5f;
}

float FeatureExtractor::calculateDynamicRange() {
    if (m_lastRMS <= 0 || m_peakLevel <= 0) return 0.0f;
    float dynamicRangeDb = 20.0f * std::log10(m_peakLevel / m_lastRMS);
    return std::clamp(dynamicRangeDb / 40.0f, 0.0f, 1.0f);
}

float FeatureExtractor::calculateBassContent() {
    if (m_lastSpectrum.empty()) return 0.5f;

    float binWidth = static_cast<float>(m_sampleRate) / m_fftSize;
    int bassCutoffBin = static_cast<int>(200.0f / binWidth);

    float bassEnergy = 0.0f;
    float totalEnergy = 0.0f;

    for (size_t i = 0; i < m_lastSpectrum.size(); ++i) {
        totalEnergy += m_lastSpectrum[i];
        if (static_cast<int>(i) < bassCutoffBin) {
            bassEnergy += m_lastSpectrum[i];
        }
    }

    return totalEnergy > 0 ? bassEnergy / totalEnergy : 0.5f;
}

} // namespace SoundBoost
