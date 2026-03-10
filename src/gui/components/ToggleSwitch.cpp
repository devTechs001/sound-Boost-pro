// src/gui/components/ToggleSwitch.cpp
#include "ToggleSwitch.hpp"
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>

namespace SoundBoost {

ToggleSwitch::ToggleSwitch(QWidget* parent) : QWidget(parent) {
    setFixedSize(60, 30);
}

void ToggleSwitch::setChecked(bool checked) {
    m_checked = checked;
    update();
}

void ToggleSwitch::setText(const QString& text) {
    m_text = text;
    update();
}

void ToggleSwitch::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int width = 50;
    int height = 26;
    int x = (this->width() - width) / 2;
    int y = (this->height() - height) / 2;

    // Background track
    QColor trackColor = m_checked ? QColor(0, 200, 255) : QColor(60, 60, 70);
    painter.setBrush(trackColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(x, y, width, height, height / 2, height / 2);

    // Thumb (circle)
    int thumbRadius = height / 2 - 4;
    int thumbX = m_checked ? (x + width - thumbRadius - 2) : (x + thumbRadius + 2);
    painter.setBrush(QColor(255, 255, 255));
    painter.drawEllipse(QPoint(thumbX, y + height / 2), thumbRadius, thumbRadius);

    // Text label
    if (!m_text.isEmpty()) {
        painter.setPen(QColor(200, 200, 200));
        painter.setFont(QFont("Arial", 9));
        painter.drawText(rect(), Qt::AlignCenter, m_text);
    }
}

void ToggleSwitch::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_checked = !m_checked;
        emit toggled(m_checked);
        update();
    }
}

} // namespace SoundBoost
