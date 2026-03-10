// tests/unit/test_effects.cpp
#include <gtest/gtest.h>
#include "core/effects/BassBooster.hpp"
#include "core/effects/Equalizer.hpp"
#include "core/effects/Compressor.hpp"
#include "core/audio/AudioBuffer.hpp"

using namespace SoundBoost;

class BassBoosterTest : public ::testing::Test {
protected:
    void SetUp() override {
        bassBooster = std::make_unique<BassBooster>();
        bassBooster->initialize(48000, 512);
    }

    std::unique_ptr<BassBooster> bassBooster;
};

TEST_F(BassBoosterTest, DefaultParameters) {
    EXPECT_FLOAT_EQ(bassBooster->getFrequency(), 80.0f);
    EXPECT_FLOAT_EQ(bassBooster->getGain(), 6.0f);
    EXPECT_FLOAT_EQ(bassBooster->getWidth(), 1.5f);
}

TEST_F(BassBoosterTest, SetParameters) {
    bassBooster->setFrequency(100.0f);
    bassBooster->setGain(12.0f);
    bassBooster->setWidth(2.0f);

    EXPECT_FLOAT_EQ(bassBooster->getFrequency(), 100.0f);
    EXPECT_FLOAT_EQ(bassBooster->getGain(), 12.0f);
    EXPECT_FLOAT_EQ(bassBooster->getWidth(), 2.0f);
}

TEST_F(BassBoosterTest, ParameterClamping) {
    bassBooster->setFrequency(5.0f);  // Below min
    EXPECT_FLOAT_EQ(bassBooster->getFrequency(), 20.0f);

    bassBooster->setFrequency(500.0f);  // Above max
    EXPECT_FLOAT_EQ(bassBooster->getFrequency(), 200.0f);

    bassBooster->setGain(-5.0f);  // Below min
    EXPECT_FLOAT_EQ(bassBooster->getGain(), 0.0f);

    bassBooster->setGain(25.0f);  // Above max
    EXPECT_FLOAT_EQ(bassBooster->getGain(), 18.0f);
}

TEST_F(BassBoosterTest, ProcessDoesNotClip) {
    AudioBuffer buffer(512, 2, 48000);
    float* data = buffer.getData();

    // Generate test tone at 60Hz
    for (size_t i = 0; i < buffer.getNumSamples(); ++i) {
        float sample = 0.5f * std::sin(2.0f * M_PI * 60.0f * i / 48000.0f);
        data[i * 2] = sample;
        data[i * 2 + 1] = sample;
    }

    bassBooster->setGain(12.0f);
    bassBooster->process(buffer);

    // Check output doesn't exceed reasonable limits
    float peak = buffer.getPeak();
    EXPECT_LT(peak, 2.0f);  // Should not be extremely boosted
}

class EqualizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        eq = std::make_unique<Equalizer>(10);
        eq->initialize(48000, 512);
    }

    std::unique_ptr<Equalizer> eq;
};

TEST_F(EqualizerTest, DefaultBandCount) {
    EXPECT_EQ(eq->getNumBands(), 10);
}

TEST_F(EqualizerTest, SetBandGain) {
    eq->setBandGain(0, 6.0f);
    EXPECT_FLOAT_EQ(eq->getBandGain(0), 6.0f);

    eq->setBandGain(5, -3.0f);
    EXPECT_FLOAT_EQ(eq->getBandGain(5), -3.0f);
}

TEST_F(EqualizerTest, FlatPreset) {
    // Set some gains
    eq->setBandGain(0, 6.0f);
    eq->setBandGain(5, -3.0f);

    // Reset to flat
    eq->setFlat();

    for (int i = 0; i < eq->getNumBands(); ++i) {
        EXPECT_FLOAT_EQ(eq->getBandGain(i), 0.0f);
    }
}

TEST_F(EqualizerTest, FrequencyResponse) {
    auto response = eq->getFrequencyResponse(512);

    EXPECT_EQ(response.size(), 512);

    // With flat EQ, response should be near 0 dB
    for (float val : response) {
        EXPECT_NEAR(val, 0.0f, 0.1f);
    }
}

class CompressorTest : public ::testing::Test {
protected:
    void SetUp() override {
        compressor = std::make_unique<Compressor>();
        compressor->initialize(48000, 512);
    }

    std::unique_ptr<Compressor> compressor;
};

TEST_F(CompressorTest, DefaultParameters) {
    EXPECT_FLOAT_EQ(compressor->getThreshold(), -20.0f);
    EXPECT_FLOAT_EQ(compressor->getRatio(), 4.0f);
    EXPECT_FLOAT_EQ(compressor->getAttack(), 10.0f);
    EXPECT_FLOAT_EQ(compressor->getRelease(), 100.0f);
}

TEST_F(CompressorTest, CompressesSilentSignal) {
    AudioBuffer buffer(512, 2, 48000);
    buffer.clear();

    compressor->process(buffer);

    // Silent input should remain silent
    EXPECT_FLOAT_EQ(buffer.getPeak(), 0.0f);
    EXPECT_FLOAT_EQ(compressor->getGainReduction(), 0.0f);
}

TEST_F(CompressorTest, CompressesLoudSignal) {
    AudioBuffer buffer(512, 2, 48000);
    float* data = buffer.getData();

    // Generate loud signal above threshold
    for (size_t i = 0; i < buffer.getSize(); ++i) {
        data[i] = 0.9f;  // Well above -20dB threshold
    }

    compressor->setThreshold(-10.0f);
    compressor->setRatio(4.0f);
    compressor->process(buffer);

    // Output should be reduced
    float outputPeak = buffer.getPeak();
    EXPECT_LT(outputPeak, 0.9f);
    EXPECT_GT(compressor->getGainReduction(), 0.0f);
}
