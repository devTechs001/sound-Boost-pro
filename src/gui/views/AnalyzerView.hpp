// src/gui/views/AnalyzerView.hpp
#pragma once

#include <QWidget>
#include <vector>

namespace SoundBoost {

class AudioEngine;
class SpectrumWidget;
class WaveformWidget;

class AnalyzerView : public QWidget {
    Q_OBJECT

public:
    explicit AnalyzerView(AudioEngine& audioEngine, QWidget* parent = nullptr);
    ~AnalyzerView() override = default;

    void updateSpectrum(const std::vector<float>& magnitudes);
    void updateLevels(float left, float right);

private:
    void setupUI();

    AudioEngine& m_audioEngine;
    SpectrumWidget* m_spectrumWidget{nullptr};
    WaveformWidget* m_waveformWidget{nullptr};
};

} // namespace SoundBoost
