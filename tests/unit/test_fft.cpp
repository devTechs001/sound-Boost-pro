// tests/unit/test_fft.cpp
#include <gtest/gtest.h>
#include "../../src/core/dsp/FFTProcessor.hpp"
#include "../../src/core/audio/AudioBuffer.hpp"

using namespace SoundBoost;

TEST(FFTProcessorTest, Initialize) {
    FFTProcessor processor;
    processor.initialize(1024);
    
    EXPECT_EQ(processor.getFFTSize(), 1024);
    EXPECT_EQ(processor.getNumBins(), 513);
}

TEST(FFTProcessorTest, ForwardTransform) {
    FFTProcessor processor;
    processor.initialize(1024);
    
    AudioBuffer buffer(1024, 1, 48000);
    // Create a simple sine wave
    for (size_t i = 0; i < 1024; ++i) {
        buffer.getData()[i] = std::sin(2.0f * M_PI * 440.0f * i / 48000.0f);
    }
    
    std::vector<std::complex<float>> spectrum;
    processor.forward(buffer, spectrum);
    
    EXPECT_EQ(spectrum.size(), 513);
}

TEST(FFTProcessorTest, MagnitudeSpectrum) {
    FFTProcessor processor;
    processor.initialize(1024);
    
    AudioBuffer buffer(1024, 1, 48000);
    for (size_t i = 0; i < 1024; ++i) {
        buffer.getData()[i] = std::sin(2.0f * M_PI * 440.0f * i / 48000.0f);
    }
    
    auto magnitudes = processor.computeMagnitudeSpectrum(buffer);
    
    EXPECT_EQ(magnitudes.size(), 513);
    
    // All magnitudes should be non-negative
    for (const auto& mag : magnitudes) {
        EXPECT_GE(mag, 0.0f);
    }
}

TEST(FFTProcessorTest, InverseTransform) {
    FFTProcessor processor;
    processor.initialize(1024);
    
    // Create spectrum
    std::vector<std::complex<float>> spectrum(513);
    spectrum[0] = 1.0f;
    
    AudioBuffer buffer(1024, 1, 48000);
    processor.inverse(spectrum, buffer);
    
    // Should complete without errors
    EXPECT_EQ(buffer.getNumSamples(), 1024);
}
