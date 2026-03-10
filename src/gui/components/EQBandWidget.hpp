// src/gui/components/EQBandWidget.hpp
#pragma once

#include <QWidget>

namespace SoundBoost {

class EQBandWidget : public QWidget {
    Q_OBJECT

public:
    explicit EQBandWidget(float frequency, QWidget* parent = nullptr);
    ~EQBandWidget() override = default;

    void setGain(float gainDb);
    float gain() const { return m_gain; }

signals:
    void gainChanged(float gain);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    float m_frequency;
    float m_gain{0.0f};
    bool m_dragging{false};
    int m_lastY{0};
};

} // namespace SoundBoost
