// src/gui/components/WaveformWidget.hpp
#pragma once

#include <QWidget>
#include <vector>

namespace SoundBoost {

class WaveformWidget : public QWidget {
    Q_OBJECT

public:
    explicit WaveformWidget(QWidget* parent = nullptr);
    ~WaveformWidget() override = default;

    void updateData(const std::vector<float>& waveform);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<float> m_waveform;
};

} // namespace SoundBoost
