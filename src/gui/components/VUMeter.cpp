// src/gui/components/VUMeter.cpp
#include "VUMeter.hpp"
#include <QPainter>
#include <QPaintEvent>
#include <cmath>

namespace SoundBoost {

VUMeter::VUMeter(QWidget* parent) : QWidget(parent) {
    setMinimumSize(30, 200);
    setMaximumWidth(60);
}

void VUMeter::setOrientation(Qt::Orientation orientation) {
    m_orientation = orientation;
    if (orientation == Qt::Horizontal) {
        setMinimumSize(200, 30);
        setMaximumHeight(60);
    } else {
        setMinimumSize(30, 200);
        setMaximumWidth(60);
    }
    update();
}

void VUMeter::setLevel(float level) {
    m_level = std::clamp(level, 0.0f, 1.0f);
    update();
}

void VUMeter::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = contentsRect();

    // Background
    painter.fillRect(rect, QColor(30, 30, 40));

    // Draw scale
    painter.setPen(QColor(100, 100, 100));
    int numMarks = 10;

    if (m_orientation == Qt::Vertical) {
        for (int i = 0; i <= numMarks; ++i) {
            int y = rect.bottom() - (i * rect.height() / numMarks);
            if (i % 2 == 0) {
                painter.drawLine(rect.left(), y, rect.left() + 8, y);
            } else {
                painter.drawLine(rect.left(), y, rect.left() + 4, y);
            }
        }

        // Draw level bar
        int barHeight = static_cast<int>(m_level * rect.height());
        QLinearGradient gradient(rect.left(), rect.bottom(), rect.left(), rect.top());
        gradient.setColorAt(0.0, QColor(0, 200, 0));
        gradient.setColorAt(0.7, QColor(255, 200, 0));
        gradient.setColorAt(0.9, QColor(255, 0, 0));

        QRect barRect(rect.left() + 8, rect.bottom() - barHeight,
                      rect.width() - 16, barHeight);
        painter.fillRect(barRect, gradient);

        // Peak indicator
        if (m_level > 0.01f) {
            int peakY = rect.bottom() - static_cast<int>(m_level * rect.height());
            painter.fillRect(rect.left() + 8, peakY - 2, rect.width() - 16, 2, QColor(255, 255, 255));
        }

        // Clip indicator
        if (m_level >= 1.0f) {
            painter.fillRect(rect.left(), rect.top(), rect.width(), 4, QColor(255, 0, 0));
        }
    } else {
        // Horizontal orientation
        for (int i = 0; i <= numMarks; ++i) {
            int x = rect.left() + (i * rect.width() / numMarks);
            if (i % 2 == 0) {
                painter.drawLine(x, rect.bottom(), x, rect.bottom() - 8);
            } else {
                painter.drawLine(x, rect.bottom(), x, rect.bottom() - 4);
            }
        }

        int barWidth = static_cast<int>(m_level * rect.width());
        QLinearGradient gradient(rect.left(), rect.top(), rect.right(), rect.top());
        gradient.setColorAt(0.0, QColor(0, 200, 0));
        gradient.setColorAt(0.7, QColor(255, 200, 0));
        gradient.setColorAt(0.9, QColor(255, 0, 0));

        QRect barRect(rect.left(), rect.top() + 4, barWidth, rect.height() - 8);
        painter.fillRect(barRect, gradient);
    }
}

} // namespace SoundBoost
