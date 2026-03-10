// src/gui/components/EQBandWidget.cpp
#include "EQBandWidget.hpp"
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <cmath>

namespace SoundBoost {

EQBandWidget::EQBandWidget(float frequency, QWidget* parent) 
    : QWidget(parent), m_frequency(frequency) {
    setMinimumSize(40, 150);
    setMaximumWidth(60);
}

void EQBandWidget::setGain(float gainDb) {
    m_gain = std::clamp(gainDb, -18.0f, 18.0f);
    update();
}

void EQBandWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = contentsRect();
    
    // Background
    painter.fillRect(rect, QColor(25, 25, 35));

    // Center line (0 dB)
    int centerY = rect.center().y();
    painter.setPen(QPen(QColor(100, 100, 100), 1, Qt::DashLine));
    painter.drawLine(rect.left(), centerY, rect.right(), centerY);

    // Draw gain indicator
    float normalized = m_gain / 18.0f; // -1 to 1
    int indicatorY = centerY - static_cast<int>(normalized * (rect.height() / 2 - 10));

    // Draw handle
    int handleSize = 20;
    QRect handleRect(rect.center().x() - handleSize / 2, 
                     indicatorY - handleSize / 2,
                     handleSize, handleSize);
    
    // Color based on gain
    QColor handleColor;
    if (m_gain > 0) {
        handleColor = QColor(0, 200, 100);
    } else if (m_gain < 0) {
        handleColor = QColor(200, 100, 0);
    } else {
        handleColor = QColor(200, 200, 200);
    }
    
    painter.setBrush(handleColor);
    painter.setPen(QPen(QColor(255, 255, 255), 2));
    painter.drawEllipse(handleRect);

    // Draw frequency label
    painter.setPen(QColor(150, 150, 150));
    painter.setFont(QFont("Arial", 7));
    QString freqText;
    if (m_frequency >= 1000) {
        freqText = QString::number(m_frequency / 1000, 'f', 1) + "k";
    } else {
        freqText = QString::number(m_frequency);
    }
    painter.drawText(rect, Qt::AlignBottom | Qt::AlignHCenter, freqText);

    // Draw gain value
    painter.drawText(rect, Qt::AlignTop | Qt::AlignHCenter, 
                     QString::number(m_gain, 'f', 1) + " dB");
}

void EQBandWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_lastY = event->globalY();
        setCursor(Qt::SizeVerCursor);
    }
}

void EQBandWidget::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging) {
        int delta = m_lastY - event->globalY();
        m_gain += delta * 0.5f;
        m_gain = std::clamp(m_gain, -18.0f, 18.0f);
        m_lastY = event->globalY();
        emit gainChanged(m_gain);
        update();
    }
}

void EQBandWidget::mouseReleaseEvent(QMouseEvent* event) {
    m_dragging = false;
    setCursor(Qt::ArrowCursor);
}

} // namespace SoundBoost
