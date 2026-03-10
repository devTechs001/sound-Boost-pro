// tests/unit/test_ml_engine.cpp
#include <gtest/gtest.h>
#include "../../src/core/ml/MLEngine.hpp"
#include "../../src/core/audio/AudioBuffer.hpp"

using namespace SoundBoost;

TEST(MLEngineTest, Construction) {
    MLEngine engine;
    EXPECT_FALSE(engine.isInitialized());
}

TEST(MLEngineTest, Initialize) {
    MLEngine engine;
    // Initialize without actual models
    bool result = engine.loadModels("models/");
    // May fail if models don't exist, but shouldn't crash
}

TEST(MLEngineTest, DetectGenre) {
    MLEngine engine;
    AudioBuffer buffer(1024, 2, 48000);
    
    std::string genre = engine.detectGenre(buffer);
    // Without models, should return "Unknown"
    EXPECT_EQ(genre, "Unknown");
}

TEST(MLEngineTest, DetectBPM) {
    MLEngine engine;
    AudioBuffer buffer(1024, 2, 48000);
    
    float bpm = engine.detectBPM(buffer);
    // Without models, should return 0
    EXPECT_FLOAT_EQ(bpm, 0.0f);
}

TEST(MLEngineTest, AdaptiveEQ) {
    MLEngine engine;
    engine.enableAdaptiveProcessing(true);
    EXPECT_TRUE(engine.isAdaptiveProcessingEnabled());
    
    engine.enableAdaptiveProcessing(false);
    EXPECT_FALSE(engine.isAdaptiveProcessingEnabled());
}
