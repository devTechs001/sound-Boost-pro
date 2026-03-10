// src/gui/views/BassBoostView.hpp
#pragma once

#include <QWidget>

namespace SoundBoost {

class AudioEngine;
class BassBooster;
class Knob;

class BassBoostView : public QWidget {
    Q_OBJECT

public:
    explicit BassBoostView(AudioEngine& audioEngine, QWidget* parent = nullptr);
    ~BassBoostView() override = default;

private slots:
    void onFrequencyChanged(float value);
    void onGainChanged(float value);
    void onWidthChanged(float value);

private:
    void setupUI();

    AudioEngine& m_audioEngine;
    BassBooster* m_bassBooster{nullptr};

    Knob* m_frequencyKnob{nullptr};
    Knob* m_gainKnob{nullptr};
    Knob* m_widthKnob{nullptr};
};

} // namespace SoundBoost
