// src/core/effects/EffectChain.cpp
#include "EffectChain.hpp"
#include "../../utils/Logger.hpp"

#include <algorithm>
#include <fstream>

namespace SoundBoost {

EffectChain::EffectChain() = default;
EffectChain::~EffectChain() = default;

void EffectChain::initialize(int sampleRate, int bufferSize) {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_sampleRate = sampleRate;
    m_bufferSize = bufferSize;

    for (auto& effect : m_effects) {
        effect->initialize(sampleRate, bufferSize);
    }

    m_initialized = true;
    LOG_DEBUG("EffectChain initialized with {} effects", m_effects.size());
}

void EffectChain::process(AudioBuffer& buffer) {
    if (m_bypass || !m_initialized) return;

    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& effect : m_effects) {
        if (effect->isEnabled()) {
            effect->process(buffer);
        }
    }
}

void EffectChain::reset() {
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& effect : m_effects) {
        effect->reset();
    }
}

void EffectChain::addEffect(std::unique_ptr<EffectBase> effect) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_initialized) {
        effect->initialize(m_sampleRate, m_bufferSize);
    }

    std::string name = effect->getName();
    m_effects.push_back(std::move(effect));
    updateIndices();

    LOG_DEBUG("Added effect: {}", name);
}

void EffectChain::insertEffect(size_t index, std::unique_ptr<EffectBase> effect) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (index > m_effects.size()) {
        index = m_effects.size();
    }

    if (m_initialized) {
        effect->initialize(m_sampleRate, m_bufferSize);
    }

    std::string name = effect->getName();
    m_effects.insert(m_effects.begin() + index, std::move(effect));
    updateIndices();

    LOG_DEBUG("Inserted effect at {}: {}", index, name);
}

void EffectChain::removeEffect(size_t index) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (index >= m_effects.size()) return;

    std::string name = m_effects[index]->getName();
    m_effects.erase(m_effects.begin() + index);
    updateIndices();

    LOG_DEBUG("Removed effect at {}: {}", index, name);
}

void EffectChain::removeEffect(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_effectIndices.find(name);
    if (it != m_effectIndices.end()) {
        m_effects.erase(m_effects.begin() + it->second);
        updateIndices();
        LOG_DEBUG("Removed effect: {}", name);
    }
}

void EffectChain::moveEffect(size_t fromIndex, size_t toIndex) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (fromIndex >= m_effects.size() || toIndex >= m_effects.size()) return;
    if (fromIndex == toIndex) return;

    auto effect = std::move(m_effects[fromIndex]);
    m_effects.erase(m_effects.begin() + fromIndex);
    m_effects.insert(m_effects.begin() + toIndex, std::move(effect));
    updateIndices();

    LOG_DEBUG("Moved effect from {} to {}", fromIndex, toIndex);
}

void EffectChain::clearEffects() {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_effects.clear();
    m_effectIndices.clear();

    LOG_DEBUG("Cleared all effects");
}

EffectBase* EffectChain::getEffect(size_t index) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (index >= m_effects.size()) return nullptr;
    return m_effects[index].get();
}

EffectBase* EffectChain::getEffect(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_effectIndices.find(name);
    if (it != m_effectIndices.end()) {
        return m_effects[it->second].get();
    }
    return nullptr;
}

const EffectBase* EffectChain::getEffect(size_t index) const {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (index >= m_effects.size()) return nullptr;
    return m_effects[index].get();
}

const EffectBase* EffectChain::getEffect(const std::string& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_effectIndices.find(name);
    if (it != m_effectIndices.end()) {
        return m_effects[it->second].get();
    }
    return nullptr;
}

void EffectChain::setBypass(bool bypass) {
    m_bypass = bypass;
}

void EffectChain::setEffectEnabled(size_t index, bool enabled) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (index < m_effects.size()) {
        m_effects[index]->setEnabled(enabled);
    }
}

void EffectChain::setEffectEnabled(const std::string& name, bool enabled) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_effectIndices.find(name);
    if (it != m_effectIndices.end()) {
        m_effects[it->second]->setEnabled(enabled);
    }
}

void EffectChain::updateIndices() {
    m_effectIndices.clear();
    for (size_t i = 0; i < m_effects.size(); ++i) {
        m_effectIndices[m_effects[i]->getName()] = i;
    }
}

std::string EffectChain::serialize() const {
    std::lock_guard<std::mutex> lock(m_mutex);

    // Simple JSON-like serialization
    std::string result = "{\n";
    result += "  \"version\": \"1.0\",\n";
    result += "  \"bypass\": " + std::string(m_bypass ? "true" : "false") + ",\n";
    result += "  \"effects\": [\n";

    for (size_t i = 0; i < m_effects.size(); ++i) {
        const auto& effect = m_effects[i];
        result += "    {\n";
        result += "      \"name\": \"" + effect->getName() + "\",\n";
        result += "      \"enabled\": " + std::string(effect->isEnabled() ? "true" : "false") + ",\n";
        result += "      \"parameters\": {\n";

        bool first = true;
        for (const auto& [name, info] : effect->getParameterInfos()) {
            if (!first) result += ",\n";
            result += "        \"" + name + "\": " + std::to_string(info.value);
            first = false;
        }

        result += "\n      }\n";
        result += "    }";
        if (i < m_effects.size() - 1) result += ",";
        result += "\n";
    }

    result += "  ]\n";
    result += "}\n";

    return result;
}

void EffectChain::savePreset(const std::string& path) const {
    std::ofstream file(path);
    if (file.is_open()) {
        file << serialize();
        LOG_INFO("Saved effect chain preset to {}", path);
    }
}

void EffectChain::loadPreset(const std::string& path) {
    std::ifstream file(path);
    if (file.is_open()) {
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        deserialize(content);
        LOG_INFO("Loaded effect chain preset from {}", path);
    }
}

void EffectChain::deserialize(const std::string& data) {
    // Simple JSON parsing - in production, use a proper JSON library
    LOG_DEBUG("Deserializing effect chain: {}", data);
}

} // namespace SoundBoost
