// src/gui/components/Knob.cpp
#include "Knob.hpp"
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <cmath>

namespace SoundBoost {

Knob::Knob(QWidget* parent) : QWidget(parent) {
    setMinimumSize(60, 60);
    setMaximumSize(100, 100);
}

void Knob::setRange(float min, float max) {
    m_min = min;
    m_max = max;
    update();
}

void Knob::setValue(float value) {
    m_value = std::clamp(value, m_min, m_max);
    update();
}

void Knob::setLabel(const QString& label) {
    m_label = label;
    update();
}

void Knob::setUnit(const QString& unit) {
    m_unit = unit;
    update();
}

void Knob::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int size = std::min(width(), height());
    int centerX = width() / 2;
    int centerY = height() / 2;
    int radius = size / 2 - 8;

    // Draw background
    painter.setBrush(QColor(30, 30, 40));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPoint(centerX, centerY), radius, radius);

    // Draw value arc
    float normalized = (m_value - m_min) / (m_max - m_min);
    float spanAngle = 270 * 16; // 270 degrees in 1/16 degrees
    float startAngle = 135 * 16;
    float currentAngle = startAngle + normalized * spanAngle;

    painter.setPen(QPen(QColor(0, 200, 255), 4));
    painter.drawArc(QRect(centerX - radius, centerY - radius, 
                          2 * radius, 2 * radius),
                    static_cast<int>(startAngle), 
                    static_cast<int>(currentAngle - startAngle));

    // Draw indicator line
    float angleRad = (currentAngle / 16.0f) * M_PI / 180.0f;
    int indicatorLength = radius - 10;
    int x2 = centerX + static_cast<int>(indicatorLength * std::sin(angleRad));
    int y2 = centerY - static_cast<int>(indicatorLength * std::cos(angleRad));
    
    painter.setPen(QPen(QColor(255, 255, 255), 2));
    painter.drawLine(centerX, centerY, x2, y2);

    // Draw center dot
    painter.setBrush(QColor(255, 255, 255));
    painter.drawEllipse(QPoint(centerX, centerY), 4, 4);

    // Draw label
    if (!m_label.isEmpty()) {
        painter.setPen(QColor(200, 200, 200));
        painter.setFont(QFont("Arial", 8));
        QString text = m_label;
        if (!m_unit.isEmpty()) {
            text += "\n" + QString::number(m_value, 'f', 1) + " " + m_unit;
        }
        painter.drawText(rect(), Qt::AlignBottom | Qt::AlignHCenter, text);
    }
}

void Knob::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_lastY = event->globalY();
        setCursor(Qt::SizeVerCursor);
    }
}

void Knob::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging) {
        int delta = m_lastY - event->globalY();
        float range = m_max - m_min;
        m_value += (delta / 100.0f) * range;
        m_value = std::clamp(m_value, m_min, m_max);
        m_lastY = event->globalY();
        emit valueChanged(m_value);
        update();
    }
}

void Knob::mouseReleaseEvent(QMouseEvent* event) {
    m_dragging = false;
    setCursor(Qt::ArrowCursor);
}

void Knob::wheelEvent(QWheelEvent* event) {
    float delta = event->angleDelta().y() / 120.0f;
    float range = m_max - m_min;
    m_value += (delta / 10.0f) * range;
    m_value = std::clamp(m_value, m_min, m_max);
    emit valueChanged(m_value);
    update();
}

} // namespace SoundBoost
