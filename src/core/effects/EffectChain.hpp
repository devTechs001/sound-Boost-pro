// src/core/effects/EffectChain.hpp
#pragma once

#include "EffectBase.hpp"
#include "../audio/AudioBuffer.hpp"

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>

namespace SoundBoost {

class EffectChain {
public:
    EffectChain();
    ~EffectChain();

    void initialize(int sampleRate, int bufferSize);
    void process(AudioBuffer& buffer);
    void reset();

    // Effect management
    void addEffect(std::unique_ptr<EffectBase> effect);
    void insertEffect(size_t index, std::unique_ptr<EffectBase> effect);
    void removeEffect(size_t index);
    void removeEffect(const std::string& name);
    void moveEffect(size_t fromIndex, size_t toIndex);
    void clearEffects();

    // Access
    EffectBase* getEffect(size_t index);
    EffectBase* getEffect(const std::string& name);
    const EffectBase* getEffect(size_t index) const;
    const EffectBase* getEffect(const std::string& name) const;

    template<typename T>
    T* getEffectAs(const std::string& name) {
        return dynamic_cast<T*>(getEffect(name));
    }

    size_t getEffectCount() const { return m_effects.size(); }

    // Bypass
    void setBypass(bool bypass);
    bool isBypassed() const { return m_bypass; }

    // Individual effect bypass
    void setEffectEnabled(size_t index, bool enabled);
    void setEffectEnabled(const std::string& name, bool enabled);

    // Preset management
    void savePreset(const std::string& path) const;
    void loadPreset(const std::string& path);

    // Serialization
    std::string serialize() const;
    void deserialize(const std::string& data);

private:
    std::vector<std::unique_ptr<EffectBase>> m_effects;
    std::unordered_map<std::string, size_t> m_effectIndices;

    int m_sampleRate{48000};
    int m_bufferSize{512};
    bool m_bypass{false};
    bool m_initialized{false};

    mutable std::mutex m_mutex;

    void updateIndices();
};

} // namespace SoundBoost
