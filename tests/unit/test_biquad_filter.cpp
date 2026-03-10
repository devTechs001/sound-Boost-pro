// tests/unit/test_biquad_filter.cpp
#include <gtest/gtest.h>
#include "../../src/core/dsp/BiquadFilter.hpp"
#include "../../src/core/dsp/FilterDesign.hpp"

using namespace SoundBoost;

TEST(BiquadFilterTest, Construction) {
    BiquadFilter filter;
    // Should initialize with default coefficients
}

TEST(BiquadFilterTest, Process) {
    BiquadCoefficients coeffs;
    coeffs.b0 = 1.0f;
    coeffs.b1 = 0.0f;
    coeffs.b2 = 0.0f;
    coeffs.a1 = 0.0f;
    coeffs.a2 = 0.0f;
    
    BiquadFilter filter(coeffs);
    
    float output = filter.process(0.5f);
    EXPECT_FLOAT_EQ(output, 0.5f);
}

TEST(BiquadFilterTest, LowPassFilter) {
    auto coeffs = FilterDesign::lowPass(48000, 1000.0f, 0.707f);
    BiquadFilter filter(coeffs);
    
    // Process a block of samples
    std::vector<float> samples(100, 1.0f);
    filter.processBlock(samples.data(), samples.size());
    
    // Output should be bounded
    for (const auto& sample : samples) {
        EXPECT_TRUE(sample >= -2.0f && sample <= 2.0f);
    }
}

TEST(BiquadFilterTest, Reset) {
    BiquadCoefficients coeffs;
    coeffs.b0 = 1.0f;
    
    BiquadFilter filter(coeffs);
    filter.process(1.0f);
    filter.reset();
    
    // After reset, state should be cleared
    float output = filter.process(0.5f);
    EXPECT_FLOAT_EQ(output, 0.5f);
}

TEST(CascadedBiquadTest, Construction) {
    CascadedBiquad cascaded(3);
    // Should have 3 sections
}

TEST(CascadedBiquadTest, Process) {
    CascadedBiquad cascaded(2);
    
    BiquadCoefficients coeffs;
    coeffs.b0 = 0.5f;
    coeffs.b1 = 0.0f;
    coeffs.b2 = 0.0f;
    coeffs.a1 = 0.0f;
    coeffs.a2 = 0.0f;
    
    cascaded.setSectionCoefficients(0, coeffs);
    cascaded.setSectionCoefficients(1, coeffs);
    
    float output = cascaded.process(1.0f);
    // Two stages of 0.5 gain = 0.25
    EXPECT_NEAR(output, 0.25f, 0.001f);
}
