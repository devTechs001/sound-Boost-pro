// src/core/effects/EffectBase.hpp
#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <variant>

namespace SoundBoost {

class AudioBuffer;

struct ParameterInfo {
    std::string name;
    float value;
    float minValue;
    float maxValue;
    std::string unit;
};

class EffectBase {
public:
    EffectBase() = default;
    virtual ~EffectBase() = default;

    virtual std::string getName() const = 0;
    virtual std::string getCategory() const = 0;

    virtual void initialize(int sampleRate, int bufferSize) = 0;
    virtual void process(AudioBuffer& buffer) = 0;
    virtual void reset() = 0;

    // Parameter management
    void registerParameter(const std::string& name, float defaultValue,
                          float minValue, float maxValue, const std::string& unit = "");

    virtual void setParameter(const std::string& name, float value);
    virtual float getParameter(const std::string& name) const;

    const std::unordered_map<std::string, ParameterInfo>& getParameterInfos() const {
        return m_parameters;
    }

    // Enable/disable
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

    // Bypass
    void setBypassed(bool bypassed) { m_bypassed = bypassed; }
    bool isBypassed() const { return m_bypassed; }

protected:
    bool m_initialized{false};
    bool m_enabled{true};
    bool m_bypassed{false};

    std::unordered_map<std::string, ParameterInfo> m_parameters;
};

} // namespace SoundBoost
