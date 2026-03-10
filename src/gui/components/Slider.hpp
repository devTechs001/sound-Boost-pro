// src/gui/components/Slider.hpp
#pragma once

#include <QWidget>

namespace SoundBoost {

class Slider : public QWidget {
    Q_OBJECT

public:
    explicit Slider(QWidget* parent = nullptr);
    ~Slider() override = default;

    void setRange(float min, float max);
    void setValue(float value);
    float value() const { return m_value; }

    void setOrientation(Qt::Orientation orientation);

signals:
    void valueChanged(float value);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    float m_value{0.0f};
    float m_min{0.0f};
    float m_max{100.0f};
    Qt::Orientation m_orientation{Qt::Horizontal};
    bool m_dragging{false};
};

} // namespace SoundBoost
