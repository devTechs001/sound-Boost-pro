// src/gui/views/HomeView.cpp
#include "HomeView.hpp"
#include "../../core/audio/AudioEngine.hpp"
#include "../../services/ServiceLocator.hpp"
#include "../components/VUMeter.hpp"
#include "../components/SpectrumWidget.hpp"

#include <QVBoxLayout>

namespace SoundBoost {

HomeView::HomeView(AudioEngine& audioEngine, ServiceLocator& services, QWidget* parent)
    : QWidget(parent)
    , m_audioEngine(audioEngine)
    , m_services(services)
{
    setupUI();
}

void HomeView::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(20);

    // Spectrum display
    m_spectrumWidget = new SpectrumWidget();
    m_spectrumWidget->setMinimumHeight(200);
    layout->addWidget(m_spectrumWidget);

    // Level meters
    auto* metersLayout = new QHBoxLayout();

    m_leftMeter = new VUMeter();
    m_leftMeter->setOrientation(Qt::Vertical);
    metersLayout->addWidget(m_leftMeter);

    m_rightMeter = new VUMeter();
    m_rightMeter->setOrientation(Qt::Vertical);
    metersLayout->addWidget(m_rightMeter);

    layout->addLayout(metersLayout);
}

void HomeView::updateSpectrum(const std::vector<float>& magnitudes) {
    if (m_spectrumWidget) {
        m_spectrumWidget->updateData(magnitudes);
    }
}

} // namespace SoundBoost
