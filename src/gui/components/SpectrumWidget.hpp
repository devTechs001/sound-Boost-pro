// src/gui/components/SpectrumWidget.hpp
#pragma once

#include <QWidget>
#include <vector>

namespace SoundBoost {

class SpectrumWidget : public QWidget {
    Q_OBJECT

public:
    explicit SpectrumWidget(QWidget* parent = nullptr);
    ~SpectrumWidget() override = default;

    void updateData(const std::vector<float>& magnitudes);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<float> m_magnitudes;
    int m_fftSize{1024};
};

} // namespace SoundBoost
