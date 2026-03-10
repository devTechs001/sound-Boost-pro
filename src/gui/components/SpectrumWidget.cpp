// src/gui/components/SpectrumWidget.cpp
#include "SpectrumWidget.hpp"
#include <QPainter>
#include <QPaintEvent>
#include <cmath>

namespace SoundBoost {

SpectrumWidget::SpectrumWidget(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(150);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

void SpectrumWidget::updateData(const std::vector<float>& magnitudes) {
    m_magnitudes = magnitudes;
    update();
}

void SpectrumWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = contentsRect();
    painter.fillRect(rect, QColor(20, 20, 30));

    if (m_magnitudes.empty()) return;

    // Draw frequency spectrum
    int numBars = std::min(static_cast<int>(m_magnitudes.size()) / 2, rect.width() / 4);
    int barWidth = rect.width() / numBars;

    for (int i = 0; i < numBars; ++i) {
        float magnitude = m_magnitudes[i * 2];
        int barHeight = static_cast<int>(magnitude * rect.height() * 3);
        barHeight = std::min(barHeight, rect.height() - 4);

        // Color based on frequency
        float hue = 200.0f - (static_cast<float>(i) / numBars) * 150.0f;
        QColor color = QColor::fromHslF(hue / 360.0f, 0.8f, 0.5f);

        QRect barRect(i * barWidth + 2, rect.bottom() - barHeight - 2,
                      barWidth - 4, barHeight);
        painter.fillRect(barRect, color);
    }

    // Draw frequency labels
    painter.setPen(QColor(150, 150, 150));
    painter.setFont(QFont("Arial", 8));
    painter.drawText(rect.left() + 5, rect.bottom() - 5, "20Hz");
    painter.drawText(rect.right() - 40, rect.bottom() - 5, "20kHz");
}

} // namespace SoundBoost
