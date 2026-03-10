// src/gui/components/DeviceSelector.hpp
#pragma once

#include <QWidget>
#include <vector>
#include <string>

class QComboBox;

namespace SoundBoost {

struct AudioDevice;

class DeviceSelector : public QWidget {
    Q_OBJECT

public:
    explicit DeviceSelector(QWidget* parent = nullptr);
    ~DeviceSelector() override = default;

    void setDevices(const std::vector<AudioDevice>& devices);

signals:
    void deviceSelected(const QString& deviceId);

private slots:
    void onCurrentTextChanged(const QString& text);

private:
    QComboBox* m_combo;
};

} // namespace SoundBoost
