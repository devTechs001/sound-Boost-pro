// src/gui/components/Knob.hpp
#pragma once

#include <QWidget>

namespace SoundBoost {

class Knob : public QWidget {
    Q_OBJECT

public:
    explicit Knob(QWidget* parent = nullptr);
    ~Knob() override = default;

    void setRange(float min, float max);
    void setValue(float value);
    float value() const { return m_value; }

    void setLabel(const QString& label);
    void setUnit(const QString& unit);

signals:
    void valueChanged(float value);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    float m_value{0.0f};
    float m_min{0.0f};
    float m_max{100.0f};
    QString m_label;
    QString m_unit;
    bool m_dragging{false};
    int m_lastY{0};
};

} // namespace SoundBoost
