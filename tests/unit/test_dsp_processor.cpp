// tests/unit/test_dsp_processor.cpp
#include <gtest/gtest.h>
#include "../../src/core/dsp/DSPProcessor.hpp"
#include "../../src/core/audio/AudioBuffer.hpp"

using namespace SoundBoost;

TEST(DSPProcessorTest, Construction) {
    DSPProcessor processor;
}

TEST(DSPProcessorTest, Initialize) {
    DSPProcessor processor;
    processor.initialize(48000, 512);
    
    EXPECT_EQ(processor.getSampleRate(), 48000);
    EXPECT_EQ(processor.getBufferSize(), 512);
}

TEST(DSPProcessorTest, ApplyLowPass) {
    DSPProcessor processor;
    processor.initialize(48000, 512);
    
    AudioBuffer buffer(512, 2, 48000);
    // Fill with test data
    for (size_t i = 0; i < buffer.getSize(); ++i) {
        buffer.getData()[i] = 0.5f;
    }
    
    processor.applyLowPass(buffer, 1000.0f, 0.707f);
    // Should complete without errors
}

TEST(DSPProcessorTest, ApplyHighPass) {
    DSPProcessor processor;
    processor.initialize(48000, 512);
    
    AudioBuffer buffer(512, 2, 48000);
    processor.applyHighPass(buffer, 100.0f, 0.707f);
}

TEST(DSPProcessorTest, Reset) {
    DSPProcessor processor;
    processor.initialize(48000, 512);
    processor.reset();
    // Should complete without errors
}
