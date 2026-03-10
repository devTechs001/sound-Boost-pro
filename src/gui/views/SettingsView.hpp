// src/gui/views/SettingsView.hpp
#pragma once

#include <QWidget>

namespace SoundBoost {

class ServiceLocator;

class SettingsView : public QWidget {
    Q_OBJECT

public:
    explicit SettingsView(ServiceLocator& services, QWidget* parent = nullptr);
    ~SettingsView() override = default;

signals:
    void themeChanged(const QString& themeName);

private slots:
    void onThemeSelected(const QString& theme);
    void onSampleRateChanged(int rate);
    void onBufferSizeChanged(int size);

private:
    void setupUI();

    ServiceLocator& m_services;
};

} // namespace SoundBoost
