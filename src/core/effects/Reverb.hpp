// src/core/effects/Reverb.hpp
#pragma once

#include "EffectBase.hpp"
#include <vector>
#include <array>

namespace SoundBoost {

class AllpassFilter {
public:
    AllpassFilter(int maxDelay = 8192);
    void setDelay(int samples);
    void setFeedback(float feedback);
    float process(float input);
    void clear();

private:
    std::vector<float> m_buffer;
    int m_writeIndex{0};
    int m_delay{0};
    float m_feedback{0.5f};
};

class CombFilter {
public:
    CombFilter(int maxDelay = 8192);
    void setDelay(int samples);
    void setFeedback(float feedback);
    void setDamping(float damping);
    float process(float input);
    void clear();

private:
    std::vector<float> m_buffer;
    std::vector<float> m_filter;
    int m_writeIndex{0};
    int m_delay{0};
    float m_feedback{0.5f};
    float m_damping{0.0f};
    float m_filterStore{0.0f};
};

class Reverb : public EffectBase {
public:
    Reverb();
    ~Reverb() override = default;

    void initialize(int sampleRate, int bufferSize) override;
    void process(AudioBuffer& buffer) override;
    void reset() override;

    std::string getName() const override { return "Reverb"; }
    std::string getCategory() const override { return "Spatial"; }

    // Parameters
    void setRoomSize(float size);
    float getRoomSize() const { return m_roomSize; }

    void setDamping(float damping);
    float getDamping() const { return m_damping; }

    void setWetLevel(float level);
    float getWetLevel() const { return m_wetLevel; }

    void setDryLevel(float level);
    float getDryLevel() const { return m_dryLevel; }

    void setWidth(float width);
    float getWidth() const { return m_width; }

    void setFreeze(bool freeze);
    bool getFreeze() const { return m_freeze; }

private:
    static constexpr int NUM_COMBS = 8;
    static constexpr int NUM_ALLPASSES = 4;

    std::array<CombFilter, NUM_COMBS> m_combFilters;
    std::array<AllpassFilter, NUM_ALLPASSES> m_allpassFilters;

    float m_roomSize{0.5f};
    float m_damping{0.5f};
    float m_wetLevel{0.33f};
    float m_dryLevel{0.67f};
    float m_width{1.0f};
    bool m_freeze{false};

    int m_sampleRate{44100};
};

} // namespace SoundBoost
