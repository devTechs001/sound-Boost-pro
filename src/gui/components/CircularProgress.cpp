// src/gui/components/CircularProgress.cpp
#include "CircularProgress.hpp"
#include <QPainter>
#include <QPaintEvent>
#include <cmath>

namespace SoundBoost {

CircularProgress::CircularProgress(QWidget* parent) : QWidget(parent) {
    setMinimumSize(100, 100);
    setMaximumSize(150, 150);
}

void CircularProgress::setValue(float value) {
    m_value = std::clamp(value, m_min, m_max);
    update();
}

void CircularProgress::setRange(float min, float max) {
    m_min = min;
    m_max = max;
    update();
}

void CircularProgress::setLabel(const QString& label) {
    m_label = label;
    update();
}

void CircularProgress::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int size = std::min(width(), height());
    int centerX = width() / 2;
    int centerY = height() / 2;
    int radius = size / 2 - 10;

    float normalized = (m_value - m_min) / (m_max - m_min);

    // Draw background circle
    painter.setPen(QPen(QColor(50, 50, 60), 8, Qt::SolidLine, Qt::RoundCap));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(QPoint(centerX, centerY), radius, radius);

    // Draw progress arc
    float spanAngle = 360 * 0.8f * normalized; // 288 degrees max
    float startAngle = 90 + 360 * 0.1f; // Start at top, offset 10%

    QPen progressPen(QColor(0, 200, 255), 8, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(progressPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawArc(QRect(centerX - radius, centerY - radius, 
                          2 * radius, 2 * radius),
                    static_cast<int>((90 - spanAngle / 2) * 16),
                    static_cast<int>(spanAngle * 16));

    // Draw center text
    if (!m_label.isEmpty()) {
        painter.setPen(QColor(255, 255, 255));
        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, m_label);
    }
}

} // namespace SoundBoost
