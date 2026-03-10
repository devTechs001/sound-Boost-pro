// src/services/AudioService.hpp
#pragma once

#include "ServiceLocator.hpp"
#include <string>
#include <memory>

namespace SoundBoost {

class AudioEngine;

class AudioService : public IService {
public:
    AudioService() = default;
    ~AudioService() override = default;

    std::string getName() const override { return "AudioService"; }

    bool initialize() override;
    void shutdown() override;

    void setAudioEngine(AudioEngine* engine) { m_audioEngine = engine; }
    AudioEngine* getAudioEngine() { return m_audioEngine; }

    void setVolume(float volume);
    float getVolume() const { return m_volume; }

    void setMuted(bool muted);
    bool isMuted() const { return m_muted; }

private:
    AudioEngine* m_audioEngine{nullptr};
    float m_volume{1.0f};
    bool m_muted{false};
};

} // namespace SoundBoost
