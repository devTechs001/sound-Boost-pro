// src/gui/views/EqualizerView.hpp
#pragma once

#include <QWidget>
#include <vector>
#include <array>
#include <memory>

namespace SoundBoost {

class AudioEngine;
class Equalizer;
class EQBandWidget;
class SpectrumWidget;

class EqualizerView : public QWidget {
    Q_OBJECT

public:
    explicit EqualizerView(AudioEngine& audioEngine, QWidget* parent = nullptr);
    ~EqualizerView() override;

    void updateSpectrum(const std::vector<float>& magnitudes);

public slots:
    void onBandGainChanged(int band, float gainDb);
    void onPresetSelected(const QString& preset);
    void onResetClicked();

private:
    void setupUI();
    void createBands();
    void loadPresets();
    void applyPreset(const std::vector<float>& gains);

    AudioEngine& m_audioEngine;
    Equalizer* m_equalizer{nullptr};

    std::vector<EQBandWidget*> m_bands;
    SpectrumWidget* m_spectrumWidget{nullptr};
    QHBoxLayout* m_bandsContainer{nullptr};

    static constexpr int NUM_BANDS = 10;
    static constexpr std::array<float, NUM_BANDS> BAND_FREQUENCIES;
};

constexpr std::array<float, EqualizerView::NUM_BANDS> EqualizerView::BAND_FREQUENCIES;

} // namespace SoundBoost
