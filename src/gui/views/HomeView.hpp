// src/gui/views/HomeView.hpp
#pragma once

#include <QWidget>
#include <vector>

namespace SoundBoost {

class AudioEngine;
class ServiceLocator;
class VUMeter;
class SpectrumWidget;

class HomeView : public QWidget {
    Q_OBJECT

public:
    explicit HomeView(AudioEngine& audioEngine, ServiceLocator& services, QWidget* parent = nullptr);
    ~HomeView() override = default;

    void updateSpectrum(const std::vector<float>& magnitudes);

private:
    void setupUI();

    AudioEngine& m_audioEngine;
    ServiceLocator& m_services;

    VUMeter* m_leftMeter{nullptr};
    VUMeter* m_rightMeter{nullptr};
    SpectrumWidget* m_spectrumWidget{nullptr};
};

} // namespace SoundBoost
