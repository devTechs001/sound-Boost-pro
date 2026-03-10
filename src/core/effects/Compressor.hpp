// src/core/effects/Compressor.hpp
#pragma once

#include "EffectBase.hpp"
#include <vector>
#include <atomic>

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
    float getThreshold() const { return m_threshold; }

    void setRatio(float ratio);
    float getRatio() const { return m_ratio; }

    void setAttack(float attackMs);
    float getAttack() const { return m_attack; }

    void setRelease(float releaseMs);
    float getRelease() const { return m_release; }

    void setKnee(float kneeDb);
    float getKnee() const { return m_knee; }

    void setMakeupGain(float gainDb);
    float getMakeupGain() const { return m_makeupGain; }

    void setAutoMakeup(bool enabled);
    bool isAutoMakeup() const { return m_autoMakeup; }

    void setLookahead(float lookaheadMs);
    float getLookahead() const { return m_lookahead; }

    void setSidechain(bool enabled);
    bool isSidechainEnabled() const { return m_sidechainEnabled; }

    // Metering
    float getGainReduction() const { return m_currentGainReduction.load(); }
    float getInputLevel() const { return m_inputLevel.load(); }
    float getOutputLevel() const { return m_outputLevel.load(); }

private:
    void updateCoefficients();
    float computeGain(float inputLevel);
    float softKnee(float inputDb, float thresholdDb, float kneeDb, float ratio);

    // Parameters
    float m_threshold{-20.0f};      // dB
    float m_ratio{4.0f};            // :1
    float m_attack{10.0f};          // ms
    float m_release{100.0f};        // ms
    float m_knee{6.0f};             // dB
    float m_makeupGain{0.0f};       // dB
    float m_lookahead{0.0f};        // ms
    bool m_autoMakeup{false};
    bool m_sidechainEnabled{false};

    // Coefficients
    float m_attackCoeff{0.0f};
    float m_releaseCoeff{0.0f};
    float m_makeupLinear{1.0f};

    // State
    float m_envelope{0.0f};
    std::vector<float> m_lookaheadBuffer;
    size_t m_lookaheadWritePos{0};
    size_t m_lookaheadSamples{0};

    // Metering
    std::atomic<float> m_currentGainReduction{0.0f};
    std::atomic<float> m_inputLevel{0.0f};
    std::atomic<float> m_outputLevel{0.0f};

    int m_sampleRate{48000};
};

} // namespace SoundBoost
