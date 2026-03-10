// src/core/effects/Equalizer.hpp
#pragma once

#include "EffectBase.hpp"
#include "../dsp/BiquadFilter.hpp"
#include <vector>

namespace SoundBoost {

class Equalizer : public EffectBase {
public:
    Equalizer(int numBands = 10);
    ~Equalizer() override = default;

    void initialize(int sampleRate, int bufferSize) override;
    void process(AudioBuffer& buffer) override;
    void reset() override;

    std::string getName() const override { return "Equalizer"; }
    std::string getCategory() const override { return "Tone"; }

    // Band management
    void setNumBands(int numBands);
    int getNumBands() const { return m_numBands; }

    void setBandFrequency(int band, float frequency);
    float getBandFrequency(int band) const;

    void setBandGain(int band, float gainDb);
    float getBandGain(int band) const;

    void setBandQ(int band, float q);
    float getBandQ(int band) const;

    // Preset frequencies (10-band ISO)
    static const std::vector<float>& getPresetFrequencies() {
        static const std::vector<float> freqs = {
            31.0f, 62.0f, 125.0f, 250.0f, 500.0f,
            1000.0f, 2000.0f, 4000.0f, 8000.0f, 16000.0f
        };
        return freqs;
    }

private:
    void updateFilters();

    int m_numBands{10};
    std::vector<float> m_frequencies;
    std::vector<float> m_gains;
    std::vector<float> m_qValues;

    std::vector<std::vector<BiquadFilter>> m_filters; // Per channel
    int m_sampleRate{48000};
};

} // namespace SoundBoost
