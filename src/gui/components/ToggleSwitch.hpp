// src/gui/components/ToggleSwitch.hpp
#pragma once

#include <QWidget>

namespace SoundBoost {

class ToggleSwitch : public QWidget {
    Q_OBJECT

public:
    explicit ToggleSwitch(QWidget* parent = nullptr);
    ~ToggleSwitch() override = default;

    void setChecked(bool checked);
    bool isChecked() const { return m_checked; }

    void setText(const QString& text);

signals:
    void toggled(bool checked);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    bool m_checked{false};
    QString m_text;
};

} // namespace SoundBoost
