// tests/unit/test_audio_buffer.cpp
#include <gtest/gtest.h>
#include "core/audio/AudioBuffer.hpp"

using namespace SoundBoost;

class AudioBufferTest : public ::testing::Test {
protected:
    void SetUp() override {
        buffer = std::make_unique<AudioBuffer>(512, 2, 48000);
    }

    std::unique_ptr<AudioBuffer> buffer;
};

TEST_F(AudioBufferTest, Construction) {
    EXPECT_EQ(buffer->getNumSamples(), 512);
    EXPECT_EQ(buffer->getNumChannels(), 2);
    EXPECT_EQ(buffer->getSampleRate(), 48000);
    EXPECT_EQ(buffer->getSize(), 1024);
}

TEST_F(AudioBufferTest, Clear) {
    // Fill with data
    float* data = buffer->getData();
    for (size_t i = 0; i < buffer->getSize(); ++i) {
        data[i] = 1.0f;
    }

    buffer->clear();

    for (size_t i = 0; i < buffer->getSize(); ++i) {
        EXPECT_FLOAT_EQ(data[i], 0.0f);
    }
}

TEST_F(AudioBufferTest, ApplyGain) {
    float* data = buffer->getData();
    for (size_t i = 0; i < buffer->getSize(); ++i) {
        data[i] = 0.5f;
    }

    buffer->applyGain(2.0f);

    for (size_t i = 0; i < buffer->getSize(); ++i) {
        EXPECT_FLOAT_EQ(data[i], 1.0f);
    }
}

TEST_F(AudioBufferTest, GetPeak) {
    float* data = buffer->getData();
    buffer->clear();
    data[100] = 0.75f;
    data[500] = -0.9f;

    EXPECT_FLOAT_EQ(buffer->getPeak(), 0.9f);
}

TEST_F(AudioBufferTest, GetRMS) {
    float* data = buffer->getData();
    for (size_t i = 0; i < buffer->getSize(); ++i) {
        data[i] = 0.5f;
    }

    EXPECT_NEAR(buffer->getRMS(), 0.5f, 0.001f);
}

TEST_F(AudioBufferTest, CopyFrom) {
    AudioBuffer source(512, 2, 48000);
    float* srcData = source.getData();
    for (size_t i = 0; i < source.getSize(); ++i) {
        srcData[i] = static_cast<float>(i) / source.getSize();
    }

    buffer->copyFrom(source);

    float* dstData = buffer->getData();
    for (size_t i = 0; i < buffer->getSize(); ++i) {
        EXPECT_FLOAT_EQ(dstData[i], srcData[i]);
    }
}

TEST_F(AudioBufferTest, AddFrom) {
    // Initialize buffer with 0.5
    float* data = buffer->getData();
    for (size_t i = 0; i < buffer->getSize(); ++i) {
        data[i] = 0.5f;
    }

    // Create source with 0.25
    AudioBuffer source(512, 2, 48000);
    float* srcData = source.getData();
    for (size_t i = 0; i < source.getSize(); ++i) {
        srcData[i] = 0.25f;
    }

    buffer->addFrom(source, 1.0f);

    for (size_t i = 0; i < buffer->getSize(); ++i) {
        EXPECT_FLOAT_EQ(data[i], 0.75f);
    }
}

TEST_F(AudioBufferTest, Normalize) {
    float* data = buffer->getData();
    buffer->clear();
    data[0] = 0.5f;
    data[1] = -0.25f;

    buffer->normalize(1.0f);

    EXPECT_FLOAT_EQ(buffer->getPeak(), 1.0f);
}
