// src/core/effects/EffectBase.cpp
#include "EffectBase.hpp"
#include <algorithm>

namespace SoundBoost {

void EffectBase::registerParameter(const std::string& name, float defaultValue,
                                   float minValue, float maxValue, const std::string& unit) {
    ParameterInfo info;
    info.name = name;
    info.value = std::clamp(defaultValue, minValue, maxValue);
    info.minValue = minValue;
    info.maxValue = maxValue;
    info.unit = unit;
    m_parameters[name] = info;
}

void EffectBase::setParameter(const std::string& name, float value) {
    auto it = m_parameters.find(name);
    if (it != m_parameters.end()) {
        it->second.value = std::clamp(value, it->second.minValue, it->second.maxValue);
    }
}

float EffectBase::getParameter(const std::string& name) const {
    auto it = m_parameters.find(name);
    if (it != m_parameters.end()) {
        return it->second.value;
    }
    return 0.0f;
}

} // namespace SoundBoost
