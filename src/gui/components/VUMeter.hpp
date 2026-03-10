// src/gui/components/VUMeter.hpp
#pragma once

#include <QWidget>

namespace SoundBoost {

class VUMeter : public QWidget {
    Q_OBJECT

public:
    explicit VUMeter(QWidget* parent = nullptr);
    ~VUMeter() override = default;

    void setOrientation(Qt::Orientation orientation);
    void setLevel(float level);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Qt::Orientation m_orientation{Qt::Vertical};
    float m_level{0.0f};
};

} // namespace SoundBoost
