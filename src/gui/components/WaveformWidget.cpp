// src/gui/components/WaveformWidget.cpp
#include "WaveformWidget.hpp"
#include <QPainter>
#include <QPaintEvent>

namespace SoundBoost {

WaveformWidget::WaveformWidget(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(100);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

void WaveformWidget::updateData(const std::vector<float>& waveform) {
    m_waveform = waveform;
    update();
}

void WaveformWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = contentsRect();
    painter.fillRect(rect, QColor(20, 20, 30));

    if (m_waveform.empty()) return;

    painter.setPen(QPen(QColor(0, 200, 255), 2));

    int centerY = rect.center().y();
    float scale = rect.height() / 2.0f;

    // Draw waveform
    QPainterPath path;
    int step = std::max(1, static_cast<int>(m_waveform.size()) / rect.width());

    bool first = true;
    for (int x = 0; x < rect.width() && x * step < static_cast<int>(m_waveform.size()); ++x) {
        float sample = m_waveform[x * step];
        int y = centerY - static_cast<int>(sample * scale);

        if (first) {
            path.moveTo(x, y);
            first = false;
        } else {
            path.lineTo(x, y);
        }
    }

    painter.drawPath(path);

    // Draw center line
    painter.setPen(QPen(QColor(50, 50, 60), 1));
    painter.drawLine(rect.left(), centerY, rect.right(), centerY);
}

} // namespace SoundBoost
