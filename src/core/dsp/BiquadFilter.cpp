// src/core/dsp/BiquadFilter.cpp
#include "BiquadFilter.hpp"
#include <cmath>

namespace SoundBoost {

BiquadFilter::BiquadFilter(const BiquadCoefficients& coeffs)
    : m_coeffs(coeffs)
{
}

void BiquadFilter::setCoefficients(const BiquadCoefficients& coeffs) {
    m_coeffs = coeffs;
}

float BiquadFilter::process(float input) {
    // Direct Form II Transposed
    float output = m_coeffs.b0 * input + m_z1;
    m_z1 = m_coeffs.b1 * input - m_coeffs.a1 * output + m_z2;
    m_z2 = m_coeffs.b2 * input - m_coeffs.a2 * output;
    return output;
}

void BiquadFilter::processBlock(float* data, size_t numSamples) {
    for (size_t i = 0; i < numSamples; ++i) {
        data[i] = process(data[i]);
    }
}

void BiquadFilter::processBlock(const float* input, float* output, size_t numSamples) {
    for (size_t i = 0; i < numSamples; ++i) {
        output[i] = process(input[i]);
    }
}

void BiquadFilter::reset() {
    m_z1 = 0.0f;
    m_z2 = 0.0f;
}

// CascadedBiquad implementation
CascadedBiquad::CascadedBiquad(int numSections) {
    setNumSections(numSections);
}

void CascadedBiquad::setNumSections(int numSections) {
    m_sections.resize(numSections);
}

void CascadedBiquad::setSectionCoefficients(int section, const BiquadCoefficients& coeffs) {
    if (section >= 0 && section < static_cast<int>(m_sections.size())) {
        m_sections[section].setCoefficients(coeffs);
    }
}

float CascadedBiquad::process(float input) {
    float output = input;
    for (auto& section : m_sections) {
        output = section.process(output);
    }
    return output;
}

void CascadedBiquad::processBlock(float* data, size_t numSamples) {
    for (auto& section : m_sections) {
        section.processBlock(data, numSamples);
    }
}

void CascadedBiquad::reset() {
    for (auto& section : m_sections) {
        section.reset();
    }
}

} // namespace SoundBoost
