// src/core/effects/BassBooster.hpp
#pragma once

#include "EffectBase.hpp"
#include "../dsp/BiquadFilter.hpp"
#include <vector>

namespace SoundBoost {

class BassBooster : public EffectBase {
public:
    BassBooster();
    ~BassBooster() override = default;

    void initialize(int sampleRate, int bufferSize) override;
    void process(AudioBuffer& buffer) override;
    void reset() override;

    std::string getName() const override { return "Bass Booster"; }
    std::string getCategory() const override { return "Tone"; }

    // Parameters
    void setFrequency(float frequency);
    float getFrequency() const { return m_frequency; }

    void setGain(float gainDb);
    float getGain() const { return m_gainDb; }

    void setWidth(float width);
    float getWidth() const { return m_width; }

    void setHarmonics(float amount);
    float getHarmonics() const { return m_harmonics; }

    void setSubBass(bool enabled);
    bool getSubBass() const { return m_subBassEnabled; }

protected:
    void updateFilters();
    void processHarmonics(AudioBuffer& buffer);
    void processSubBass(AudioBuffer& buffer);

private:
    float m_frequency{80.0f};      // Center frequency in Hz
    float m_gainDb{6.0f};          // Boost amount in dB
    float m_width{1.5f};           // Filter Q (bandwidth)
    float m_harmonics{0.0f};       // Harmonic enhancement amount
    bool m_subBassEnabled{false};  // Sub-bass synthesis

    // Multi-band processing
    std::vector<BiquadFilter> m_lowShelfFilters;  // Per channel
    std::vector<BiquadFilter> m_peakFilters;      // Per channel
    std::vector<BiquadFilter> m_subBassFilters;   // For sub-bass synthesis

    // Harmonic generator state
    std::vector<float> m_harmonicState;

    int m_sampleRate{48000};
    int m_numChannels{2};
};

} // namespace SoundBoost
