// src/core/dsp/FilterDesign.cpp
#include "FilterDesign.hpp"
#include <algorithm>

namespace SoundBoost {

float FilterDesign::prewarp(float frequency, float sampleRate) {
    return std::tan(PI * frequency / sampleRate);
}

BiquadCoefficients FilterDesign::lowPass(float sampleRate, float cutoff, float Q) {
    BiquadCoefficients coeffs;

    float w0 = 2.0f * PI * cutoff / sampleRate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / (2.0f * Q);

    float b0 = (1.0f - cosw0) / 2.0f;
    float b1 = 1.0f - cosw0;
    float b2 = (1.0f - cosw0) / 2.0f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cosw0;
    float a2 = 1.0f - alpha;

    coeffs.b0 = b0 / a0;
    coeffs.b1 = b1 / a0;
    coeffs.b2 = b2 / a0;
    coeffs.a1 = a1 / a0;
    coeffs.a2 = a2 / a0;

    return coeffs;
}

BiquadCoefficients FilterDesign::highPass(float sampleRate, float cutoff, float Q) {
    BiquadCoefficients coeffs;

    float w0 = 2.0f * PI * cutoff / sampleRate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / (2.0f * Q);

    float b0 = (1.0f + cosw0) / 2.0f;
    float b1 = -(1.0f + cosw0);
    float b2 = (1.0f + cosw0) / 2.0f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cosw0;
    float a2 = 1.0f - alpha;

    coeffs.b0 = b0 / a0;
    coeffs.b1 = b1 / a0;
    coeffs.b2 = b2 / a0;
    coeffs.a1 = a1 / a0;
    coeffs.a2 = a2 / a0;

    return coeffs;
}

BiquadCoefficients FilterDesign::bandPass(float sampleRate, float center, float bandwidth) {
    BiquadCoefficients coeffs;

    float w0 = 2.0f * PI * center / sampleRate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 * std::sinh(std::log(2.0f) / 2.0f * bandwidth * w0 / sinw0);

    float b0 = alpha;
    float b1 = 0.0f;
    float b2 = -alpha;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cosw0;
    float a2 = 1.0f - alpha;

    coeffs.b0 = b0 / a0;
    coeffs.b1 = b1 / a0;
    coeffs.b2 = b2 / a0;
    coeffs.a1 = a1 / a0;
    coeffs.a2 = a2 / a0;

    return coeffs;
}

BiquadCoefficients FilterDesign::notch(float sampleRate, float center, float Q) {
    BiquadCoefficients coeffs;

    float w0 = 2.0f * PI * center / sampleRate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / (2.0f * Q);

    float b0 = 1.0f;
    float b1 = -2.0f * cosw0;
    float b2 = 1.0f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cosw0;
    float a2 = 1.0f - alpha;

    coeffs.b0 = b0 / a0;
    coeffs.b1 = b1 / a0;
    coeffs.b2 = b2 / a0;
    coeffs.a1 = a1 / a0;
    coeffs.a2 = a2 / a0;

    return coeffs;
}

BiquadCoefficients FilterDesign::peakEQ(float sampleRate, float center, float gainDb, float Q) {
    BiquadCoefficients coeffs;

    float A = std::pow(10.0f, gainDb / 40.0f);
    float w0 = 2.0f * PI * center / sampleRate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / (2.0f * Q);

    float b0 = 1.0f + alpha * A;
    float b1 = -2.0f * cosw0;
    float b2 = 1.0f - alpha * A;
    float a0 = 1.0f + alpha / A;
    float a1 = -2.0f * cosw0;
    float a2 = 1.0f - alpha / A;

    coeffs.b0 = b0 / a0;
    coeffs.b1 = b1 / a0;
    coeffs.b2 = b2 / a0;
    coeffs.a1 = a1 / a0;
    coeffs.a2 = a2 / a0;

    return coeffs;
}

BiquadCoefficients FilterDesign::lowShelf(float sampleRate, float cutoff, float gainDb, float slope) {
    BiquadCoefficients coeffs;

    float A = std::pow(10.0f, gainDb / 40.0f);
    float w0 = 2.0f * PI * cutoff / sampleRate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / 2.0f * std::sqrt((A + 1.0f / A) * (1.0f / slope - 1.0f) + 2.0f);
    float sqrtA = std::sqrt(A);

    float b0 = A * ((A + 1.0f) - (A - 1.0f) * cosw0 + 2.0f * sqrtA * alpha);
    float b1 = 2.0f * A * ((A - 1.0f) - (A + 1.0f) * cosw0);
    float b2 = A * ((A + 1.0f) - (A - 1.0f) * cosw0 - 2.0f * sqrtA * alpha);
    float a0 = (A + 1.0f) + (A - 1.0f) * cosw0 + 2.0f * sqrtA * alpha;
    float a1 = -2.0f * ((A - 1.0f) + (A + 1.0f) * cosw0);
    float a2 = (A + 1.0f) + (A - 1.0f) * cosw0 - 2.0f * sqrtA * alpha;

    coeffs.b0 = b0 / a0;
    coeffs.b1 = b1 / a0;
    coeffs.b2 = b2 / a0;
    coeffs.a1 = a1 / a0;
    coeffs.a2 = a2 / a0;

    return coeffs;
}

BiquadCoefficients FilterDesign::highShelf(float sampleRate, float cutoff, float gainDb, float slope) {
    BiquadCoefficients coeffs;

    float A = std::pow(10.0f, gainDb / 40.0f);
    float w0 = 2.0f * PI * cutoff / sampleRate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / 2.0f * std::sqrt((A + 1.0f / A) * (1.0f / slope - 1.0f) + 2.0f);
    float sqrtA = std::sqrt(A);

    float b0 = A * ((A + 1.0f) + (A - 1.0f) * cosw0 + 2.0f * sqrtA * alpha);
    float b1 = -2.0f * A * ((A - 1.0f) + (A + 1.0f) * cosw0);
    float b2 = A * ((A + 1.0f) + (A - 1.0f) * cosw0 - 2.0f * sqrtA * alpha);
    float a0 = (A + 1.0f) - (A - 1.0f) * cosw0 + 2.0f * sqrtA * alpha;
    float a1 = 2.0f * ((A - 1.0f) - (A + 1.0f) * cosw0);
    float a2 = (A + 1.0f) - (A - 1.0f) * cosw0 - 2.0f * sqrtA * alpha;

    coeffs.b0 = b0 / a0;
    coeffs.b1 = b1 / a0;
    coeffs.b2 = b2 / a0;
    coeffs.a1 = a1 / a0;
    coeffs.a2 = a2 / a0;

    return coeffs;
}

std::vector<BiquadCoefficients> FilterDesign::butterworthLowPass(
    float sampleRate, float cutoff, int order) {

    std::vector<BiquadCoefficients> sections;
    int numSections = (order + 1) / 2;
    sections.reserve(numSections);

    for (int i = 0; i < numSections; ++i) {
        float Q;
        if (order % 2 == 1 && i == 0) {
            // First-order section for odd-order filters
            Q = 0.5f;
        } else {
            // Butterworth Q values
            int k = (order % 2 == 1) ? i : i + 1;
            float angle = PI * (2.0f * k - 1.0f) / (2.0f * order);
            Q = 1.0f / (2.0f * std::cos(angle));
        }

        sections.push_back(lowPass(sampleRate, cutoff, Q));
    }

    return sections;
}

std::vector<BiquadCoefficients> FilterDesign::butterworthHighPass(
    float sampleRate, float cutoff, int order) {

    std::vector<BiquadCoefficients> sections;
    int numSections = (order + 1) / 2;
    sections.reserve(numSections);

    for (int i = 0; i < numSections; ++i) {
        float Q;
        if (order % 2 == 1 && i == 0) {
            Q = 0.5f;
        } else {
            int k = (order % 2 == 1) ? i : i + 1;
            float angle = PI * (2.0f * k - 1.0f) / (2.0f * order);
            Q = 1.0f / (2.0f * std::cos(angle));
        }

        sections.push_back(highPass(sampleRate, cutoff, Q));
    }

    return sections;
}

std::pair<std::vector<BiquadCoefficients>, std::vector<BiquadCoefficients>>
FilterDesign::linkwitzRileyCrossover(float sampleRate, float crossoverFreq, int order) {
    // LR filters are cascaded Butterworth filters
    // For LR4 (order=4), we cascade two Butterworth LP/HP filters
    auto lowPassSections = butterworthLowPass(sampleRate, crossoverFreq, order / 2);
    auto highPassSections = butterworthHighPass(sampleRate, crossoverFreq, order / 2);

    // Cascade each section with itself for Linkwitz-Riley response
    std::vector<BiquadCoefficients> lpResult, hpResult;
    for (const auto& section : lowPassSections) {
        lpResult.push_back(section);
        lpResult.push_back(section); // Cascade
    }
    for (const auto& section : highPassSections) {
        hpResult.push_back(section);
        hpResult.push_back(section); // Cascade
    }

    return {lpResult, hpResult};
}

} // namespace SoundBoost
