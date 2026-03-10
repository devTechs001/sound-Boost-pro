// tests/unit/test_effects.cpp
#include <gtest/gtest.h>
#include "../../src/core/effects/EffectChain.hpp"
#include "../../src/core/effects/BassBooster.hpp"
#include "../../src/core/audio/AudioBuffer.hpp"

using namespace SoundBoost;

TEST(EffectChainTest, Construction) {
    EffectChain chain;
    EXPECT_EQ(chain.getEffectCount(), 0);
}

TEST(EffectChainTest, AddEffect) {
    EffectChain chain;
    chain.initialize(48000, 512);
    
    auto bassBooster = std::make_unique<BassBooster>();
    chain.addEffect(std::move(bassBooster));
    
    EXPECT_EQ(chain.getEffectCount(), 1);
    EXPECT_NE(chain.getEffect("Bass Booster"), nullptr);
}

TEST(EffectChainTest, Process) {
    EffectChain chain;
    chain.initialize(48000, 512);
    
    auto bassBooster = std::make_unique<BassBooster>();
    bassBooster->setGain(6.0f);
    chain.addEffect(std::move(bassBooster));
    
    AudioBuffer buffer(512, 2, 48000);
    // Fill with test data
    for (size_t i = 0; i < buffer.getSize(); ++i) {
        buffer.getData()[i] = 0.5f;
    }
    
    chain.process(buffer);
    // Processing should complete without errors
}

TEST(EffectChainTest, Bypass) {
    EffectChain chain;
    chain.setBypass(true);
    EXPECT_TRUE(chain.isBypassed());
    
    chain.setBypass(false);
    EXPECT_FALSE(chain.isBypassed());
}

TEST(EffectChainTest, Serialize) {
    EffectChain chain;
    chain.initialize(48000, 512);
    
    auto bassBooster = std::make_unique<BassBooster>();
    chain.addEffect(std::move(bassBooster));
    
    std::string serialized = chain.serialize();
    EXPECT_FALSE(serialized.empty());
    EXPECT_TRUE(serialized.find("Bass Booster") != std::string::npos);
}

TEST(BassBoosterTest, Construction) {
    BassBooster booster;
    EXPECT_EQ(booster.getName(), "Bass Booster");
    EXPECT_EQ(booster.getCategory(), "Tone");
}

TEST(BassBoosterTest, Parameters) {
    BassBooster booster;
    
    booster.setFrequency(100.0f);
    EXPECT_FLOAT_EQ(booster.getFrequency(), 100.0f);
    
    booster.setGain(9.0f);
    EXPECT_FLOAT_EQ(booster.getGain(), 9.0f);
    
    booster.setWidth(2.0f);
    EXPECT_FLOAT_EQ(booster.getWidth(), 2.0f);
}
