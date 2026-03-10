// src/gui/MainWindow.hpp
#pragma once

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <memory>
#include <vector>

namespace SoundBoost {

class AudioEngine;
class ServiceLocator;

namespace Ui { class MainWindow; }

class HomeView;
class EqualizerView;
class BassBoostView;
class EffectsView;
class AnalyzerView;
class SettingsView;
class MLInsightsView;
class ThemeManager;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(AudioEngine& audioEngine, ServiceLocator& services, QWidget* parent = nullptr);
    ~MainWindow() override;

    bool initialize();
    void minimizeToTray();

public slots:
    void updateAudioLevels(float left, float right);
    void updateSpectrum(const std::vector<float>& magnitudes);
    void showNotification(const QString& title, const QString& message);

protected:
    void closeEvent(QCloseEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void onNavigationClicked(int index);
    void onMasterVolumeChanged(int value);
    void onMasterToggled(bool enabled);
    void onBypassToggled(bool bypass);
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onThemeChanged(const QString& themeName);
    void onPresetSelected(const QString& presetName);

private:
    void setupUI();
    void setupNavigation();
    void setupTrayIcon();
    void setupShortcuts();
    void loadSettings();
    void saveSettings();
    void createViews();
    void connectSignals();
    void updateNavigationButtons(int index);
    void handleSettingChange(const std::string& key, const QVariant& value);

    std::unique_ptr<Ui::MainWindow> m_ui;

    AudioEngine& m_audioEngine;
    ServiceLocator& m_services;

    std::unique_ptr<HomeView> m_homeView;
    std::unique_ptr<EqualizerView> m_equalizerView;
    std::unique_ptr<BassBoostView> m_bassBoostView;
    std::unique_ptr<EffectsView> m_effectsView;
    std::unique_ptr<AnalyzerView> m_analyzerView;
    std::unique_ptr<SettingsView> m_settingsView;
    std::unique_ptr<MLInsightsView> m_mlInsightsView;

    std::unique_ptr<ThemeManager> m_themeManager;
    std::unique_ptr<QSystemTrayIcon> m_trayIcon;

    int m_currentViewIndex{0};
    bool m_minimizeToTray{true};
};

} // namespace SoundBoost
