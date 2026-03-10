// src/gui/components/Slider.cpp
#include "Slider.hpp"
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <cmath>

namespace SoundBoost {

Slider::Slider(QWidget* parent) : QWidget(parent) {
    setMinimumSize(100, 30);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void Slider::setRange(float min, float max) {
    m_min = min;
    m_max = max;
    update();
}

void Slider::setValue(float value) {
    m_value = std::clamp(value, m_min, m_max);
    update();
}

void Slider::setOrientation(Qt::Orientation orientation) {
    m_orientation = orientation;
    if (orientation == Qt::Vertical) {
        setMinimumSize(30, 100);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    } else {
        setMinimumSize(100, 30);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    update();
}

void Slider::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = contentsRect();
    float normalized = (m_value - m_min) / (m_max - m_min);

    // Draw groove
    painter.fillRect(rect, QColor(40, 40, 50));

    // Draw filled portion
    if (m_orientation == Qt::Horizontal) {
        int fillWidth = static_cast<int>(normalized * rect.width());
        QLinearGradient gradient(rect.left(), 0, fillWidth, 0);
        gradient.setColorAt(0.0, QColor(0, 200, 255));
        gradient.setColorAt(1.0, QColor(0, 150, 200));
        painter.fillRect(QRect(rect.left(), rect.top(), fillWidth, rect.height()), gradient);

        // Draw handle
        int handleX = rect.left() + static_cast<int>(normalized * rect.width());
        painter.setBrush(QColor(255, 255, 255));
        painter.setPen(QPen(QColor(100, 100, 100), 2));
        painter.drawEllipse(QPoint(handleX, rect.center().y()), 8, 8);
    } else {
        int fillHeight = static_cast<int>(normalized * rect.height());
        QLinearGradient gradient(0, rect.bottom(), 0, rect.bottom() - fillHeight);
        gradient.setColorAt(0.0, QColor(0, 200, 255));
        gradient.setColorAt(1.0, QColor(0, 150, 200));
        painter.fillRect(QRect(rect.left(), rect.bottom() - fillHeight, 
                               rect.width(), fillHeight), gradient);

        // Draw handle
        int handleY = rect.bottom() - static_cast<int>(normalized * rect.height());
        painter.setBrush(QColor(255, 255, 255));
        painter.setPen(QPen(QColor(100, 100, 100), 2));
        painter.drawEllipse(QPoint(rect.center().x(), handleY), 8, 8);
    }
}

void Slider::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        updateValueFromMouse(event->pos());
    }
}

void Slider::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging) {
        updateValueFromMouse(event->pos());
    }
}

void Slider::mouseReleaseEvent(QMouseEvent* event) {
    m_dragging = false;
}

void Slider::updateValueFromMouse(const QPoint& pos) {
    float normalized;
    if (m_orientation == Qt::Horizontal) {
        normalized = static_cast<float>(pos.x()) / width();
    } else {
        normalized = 1.0f - static_cast<float>(pos.y()) / height();
    }
    normalized = std::clamp(normalized, 0.0f, 1.0f);
    m_value = m_min + normalized * (m_max - m_min);
    emit valueChanged(m_value);
    update();
}

} // namespace SoundBoost
