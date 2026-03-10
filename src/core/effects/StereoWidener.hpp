// src/core/effects/StereoWidener.hpp
#pragma once

#include "EffectBase.hpp"
#include "../dsp/BiquadFilter.hpp"

namespace SoundBoost {

class StereoWidener : public EffectBase {
public:
    StereoWidener();
    ~StereoWidener() override = default;

    void initialize(int sampleRate, int bufferSize) override;
    void process(AudioBuffer& buffer) override;
    void reset() override;

    std::string getName() const override { return "Stereo Widener"; }
    std::string getCategory() const override { return "Spatial"; }

    // Parameters
    void setWidth(float width);
    float getWidth() const { return m_width; }

    void setMix(float mix);
    float getMix() const { return m_mix; }

private:
    float m_width{1.0f};
    float m_mix{1.0f};

    BiquadFilter m_leftFilter;
    BiquadFilter m_rightFilter;
};

} // namespace SoundBoost
