// tests/unit/test_audio_buffer.cpp
#include <gtest/gtest.h>
#include "../../src/core/audio/AudioBuffer.hpp"

using namespace SoundBoost;

TEST(AudioBufferTest, Construction) {
    AudioBuffer buffer(1024, 2, 48000);
    
    EXPECT_EQ(buffer.getNumSamples(), 1024);
    EXPECT_EQ(buffer.getNumChannels(), 2);
    EXPECT_EQ(buffer.getSampleRate(), 48000);
    EXPECT_FALSE(buffer.isEmpty());
}

TEST(AudioBufferTest, Clear) {
    AudioBuffer buffer(1024, 2, 48000);
    
    // Fill with data
    float* data = buffer.getData();
    for (size_t i = 0; i < buffer.getSize(); ++i) {
        data[i] = 1.0f;
    }
    
    buffer.clear();
    
    for (size_t i = 0; i < buffer.getSize(); ++i) {
        EXPECT_FLOAT_EQ(buffer.getData()[i], 0.0f);
    }
}

TEST(AudioBufferTest, GetPeak) {
    AudioBuffer buffer(1024, 2, 48000);
    
    float* data = buffer.getData();
    data[0] = 0.5f;
    data[1] = -0.8f;
    data[2] = 0.3f;
    
    EXPECT_FLOAT_EQ(buffer.getPeak(), 0.8f);
}

TEST(AudioBufferTest, GetRMS) {
    AudioBuffer buffer(4, 1, 48000);
    
    float* data = buffer.getData();
    data[0] = 1.0f;
    data[1] = 0.0f;
    data[2] = -1.0f;
    data[3] = 0.0f;
    
    float rms = buffer.getRMS();
    EXPECT_NEAR(rms, 0.707f, 0.001f);
}

TEST(AudioBufferTest, ApplyGain) {
    AudioBuffer buffer(4, 1, 48000);
    
    float* data = buffer.getData();
    data[0] = 1.0f;
    data[1] = 0.5f;
    data[2] = 0.25f;
    data[3] = 0.0f;
    
    buffer.applyGain(0.5f);
    
    EXPECT_FLOAT_EQ(data[0], 0.5f);
    EXPECT_FLOAT_EQ(data[1], 0.25f);
    EXPECT_FLOAT_EQ(data[2], 0.125f);
    EXPECT_FLOAT_EQ(data[3], 0.0f);
}

TEST(AudioBufferTest, Normalize) {
    AudioBuffer buffer(4, 1, 48000);
    
    float* data = buffer.getData();
    data[0] = 0.25f;
    data[1] = 0.5f;
    data[2] = 0.75f;
    data[3] = 0.0f;
    
    buffer.normalize(1.0f);
    
    EXPECT_NEAR(buffer.getPeak(), 1.0f, 0.001f);
}
