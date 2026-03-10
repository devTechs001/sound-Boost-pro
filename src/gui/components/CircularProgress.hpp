// src/gui/components/CircularProgress.hpp
#pragma once

#include <QWidget>

namespace SoundBoost {

class CircularProgress : public QWidget {
    Q_OBJECT

public:
    explicit CircularProgress(QWidget* parent = nullptr);
    ~CircularProgress() override = default;

    void setValue(float value);
    void setRange(float min, float max);

    void setLabel(const QString& label);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    float m_value{0.0f};
    float m_min{0.0f};
    float m_max{100.0f};
    QString m_label;
};

} // namespace SoundBoost
