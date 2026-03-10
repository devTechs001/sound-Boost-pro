// src/gui/views/EffectsView.hpp
#pragma once

#include <QWidget>
#include <vector>

namespace SoundBoost {

class AudioEngine;
class EffectChain;
class ToggleSwitch;

class EffectsView : public QWidget {
    Q_OBJECT

public:
    explicit EffectsView(AudioEngine& audioEngine, QWidget* parent = nullptr);
    ~EffectsView() override = default;

private:
    void setupUI();

    AudioEngine& m_audioEngine;
    EffectChain* m_effectChain{nullptr};

    std::vector<ToggleSwitch*> m_effectToggles;
};

} // namespace SoundBoost
