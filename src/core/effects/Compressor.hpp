// src/core/effects/Compressor.hpp
#pragma once

#include "EffectBase.hpp"

namespace SoundBoost {

class Compressor : public EffectBase {
public:
    Compressor();
    ~Compressor() override = default;

    void initialize(int sampleRate, int bufferSize) override;
    void process(AudioBuffer& buffer) override;
    void reset() override;

    std::string getName() const override { return "Compressor"; }
    std::string getCategory() const override { return "Dynamics"; }

    // Parameters
    void setThreshold(float thresholdDb);
    float getThreshold() const { return m_thresholdDb; }

    void setRatio(float ratio);
    float getRatio() const { return m_ratio; }

    void setAttack(float attackMs);
    float getAttack() const { return m_attackMs; }

    void setRelease(float releaseMs);
    float getRelease() const { return m_releaseMs; }

    void setKnee(float kneeDb);
    float getKnee() const { return m_kneeDb; }

    void setMakeupGain(float gainDb);
    float getMakeupGain() const { return m_makeupGainDb; }

    // Metering
    float getGainReduction() const { return m_currentGainReduction; }
    float getEnvelopeLevel() const { return m_envelope; }

private:
    float computeGainReduction(float inputLevel);

    float m_thresholdDb{-20.0f};
    float m_ratio{4.0f};
    float m_attackMs{10.0f};
    float m_releaseMs{100.0f};
    float m_kneeDb{6.0f};
    float m_makeupGainDb{0.0f};

    float m_envelope{0.0f};
    float m_currentGainReduction{0.0f};

    int m_sampleRate{48000};
};

} // namespace SoundBoost
