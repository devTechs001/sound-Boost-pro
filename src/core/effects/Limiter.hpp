// src/core/effects/Limiter.hpp
#pragma once

#include "EffectBase.hpp"

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
    float getCeiling() const { return m_ceilingDb; }

    void setLookahead(float lookaheadMs);
    float getLookahead() const { return m_lookaheadMs; }

    void setRelease(float releaseMs);
    float getRelease() const { return m_releaseMs; }

private:
    float m_ceilingDb{-0.3f};
    float m_lookaheadMs{5.0f};
    float m_releaseMs{50.0f};

    int m_sampleRate{48000};
};

} // namespace SoundBoost
