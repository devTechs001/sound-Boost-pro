// src/services/AudioService.cpp
#include "AudioService.hpp"
#include "../core/audio/AudioEngine.hpp"
#include "../utils/Logger.hpp"

namespace SoundBoost {

bool AudioService::initialize() {
    LOG_INFO("AudioService initialized");
    return true;
}

void AudioService::shutdown() {
    m_audioEngine = nullptr;
    LOG_INFO("AudioService shutdown");
}

void AudioService::setVolume(float volume) {
    m_volume = std::clamp(volume, 0.0f, 1.0f);

    if (m_audioEngine && !m_muted) {
        m_audioEngine->setMasterVolume(m_volume);
    }

    LOG_DEBUG("Volume set to {}", m_volume);
}

void AudioService::setMuted(bool muted) {
    m_muted = muted;

    if (m_audioEngine) {
        if (muted) {
            m_audioEngine->setMasterVolume(0.0f);
        } else {
            m_audioEngine->setMasterVolume(m_volume);
        }
    }

    LOG_DEBUG("Mute {}", muted ? "enabled" : "disabled");
}

} // namespace SoundBoost
