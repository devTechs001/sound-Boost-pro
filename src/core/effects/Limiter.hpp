// src/core/effects/Limiter.hpp
#pragma once

#include "EffectBase.hpp"
#include <vector>
#include <atomic>

namespace SoundBoost {

class Limiter : public EffectBase {
public:
    Limiter();
    ~Limiter() override = default;

    void initialize(int sampleRate, int bufferSize) override;
    void process(AudioBuffer& buffer) override;
    void reset() override;

    std::string getName() const override { return "Limiter"; }
    std::string getCategory() const override { return "Dynamics"; }

    // Parameters
    void setCeiling(float ceilingDb);
    float getCeiling() const { return m_ceiling; }

    void setRelease(float releaseMs);
    float getRelease() const { return m_release; }

    void setLookahead(float lookaheadMs);
    float getLookahead() const { return m_lookahead; }

    void setTruePeak(bool enabled);
    bool isTruePeak() const { return m_truePeak; }

    // Metering
    float getGainReduction() const { return m_gainReduction.load(); }

private:
    void updateCoefficients();
    float interpolateSample(const float* buffer, float position, size_t bufferSize);
    float findTruePeak(const float* samples, size_t count);

    float m_ceiling{-0.3f};       // dB
    float m_release{50.0f};       // ms
    float m_lookahead{1.0f};      // ms
    bool m_truePeak{true};

    float m_ceilingLinear{1.0f};
    float m_releaseCoeff{0.0f};
    size_t m_lookaheadSamples{0};

    std::vector<std::vector<float>> m_lookaheadBuffer;
    size_t m_lookaheadWritePos{0};

    float m_currentGain{1.0f};
    std::atomic<float> m_gainReduction{0.0f};

    int m_sampleRate{48000};
    int m_numChannels{2};
};

} // namespace SoundBoost
