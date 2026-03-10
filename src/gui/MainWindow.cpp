// src/gui/MainWindow.cpp
#include "MainWindow.hpp"
#include "../core/audio/AudioEngine.hpp"
#include "../services/ServiceLocator.hpp"
#include "../services/SettingsService.hpp"
#include "../utils/Logger.hpp"

#include <QCloseEvent>
#include <QMenu>
#include <QShortcut>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QTimer>

namespace SoundBoost {

MainWindow::MainWindow(AudioEngine& audioEngine, ServiceLocator& services, QWidget* parent)
    : QMainWindow(parent)
    , m_audioEngine(audioEngine)
    , m_services(services)
    , m_themeManager(std::make_unique<ThemeManager>())
{
    setWindowTitle("SoundBoost Pro");
    setMinimumSize(1200, 800);
}

MainWindow::~MainWindow() {
    saveSettings();
}

bool MainWindow::initialize() {
    LOG_DEBUG("Initializing MainWindow");

    setupUI();
    createViews();
    setupNavigation();
    setupTrayIcon();
    setupShortcuts();
    connectSignals();
    loadSettings();

    m_themeManager->applyTheme("dark", this);

    return true;
}

void MainWindow::setupUI() {
    // Master controls would be set up here with actual UI elements
}

void MainWindow::createViews() {
    m_homeView = std::make_unique<HomeView>(m_audioEngine, m_services);
    m_equalizerView = std::make_unique<EqualizerView>(m_audioEngine);
    m_bassBoostView = std::make_unique<BassBoostView>(m_audioEngine);
    m_effectsView = std::make_unique<EffectsView>(m_audioEngine);
    m_analyzerView = std::make_unique<AnalyzerView>(m_audioEngine);
    m_settingsView = std::make_unique<SettingsView>(m_services);
    m_mlInsightsView = std::make_unique<MLInsightsView>(m_audioEngine);
}

void MainWindow::setupNavigation() {
    // Navigation setup would be done with actual UI elements
}

void MainWindow::setupTrayIcon() {
    m_trayIcon = std::make_unique<QSystemTrayIcon>(QIcon(":/icons/app_icon.png"), this);

    auto* trayMenu = new QMenu(this);

    auto* showAction = trayMenu->addAction("Show");
    connect(showAction, &QAction::triggered, this, &MainWindow::showNormal);

    auto* muteAction = trayMenu->addAction("Mute");
    muteAction->setCheckable(true);
    connect(muteAction, &QAction::toggled, [this](bool muted) {
        onMasterToggled(!muted);
    });

    trayMenu->addSeparator();

    auto* quitAction = trayMenu->addAction("Quit");
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    m_trayIcon->setContextMenu(trayMenu);
    m_trayIcon->setToolTip("SoundBoost Pro");

    connect(m_trayIcon.get(), &QSystemTrayIcon::activated,
            this, &MainWindow::onTrayIconActivated);

    m_trayIcon->show();
}

void MainWindow::setupShortcuts() {
    new QShortcut(QKeySequence("Ctrl+M"), this, [this]() {
        onMasterToggled(true);
    });

    new QShortcut(QKeySequence("Ctrl+B"), this, [this]() {
        onBypassToggled(true);
    });

    new QShortcut(QKeySequence("Ctrl+Q"), this, qApp, &QApplication::quit);
}

void MainWindow::connectSignals() {
    // Signal connections would be made here
}

void MainWindow::loadSettings() {
    auto& settings = m_services.get<SettingsService>();

    restoreGeometry(settings.get<QByteArray>("window.geometry", QByteArray()));
    restoreState(settings.get<QByteArray>("window.state", QByteArray()));

    m_minimizeToTray = settings.get<bool>("ui.minimize_to_tray", true);
}

void MainWindow::saveSettings() {
    auto& settings = m_services.get<SettingsService>();

    settings.set("window.geometry", saveGeometry());
    settings.set("window.state", saveState());
}

void MainWindow::onNavigationClicked(int index) {
    if (index == m_currentViewIndex) return;
    m_currentViewIndex = index;
    updateNavigationButtons(index);
}

void MainWindow::onMasterVolumeChanged(int value) {
    float volume = value / 100.0f;
    m_audioEngine.setMasterVolume(volume);
}

void MainWindow::onMasterToggled(bool enabled) {
    m_audioEngine.setMasterEnabled(enabled);
}

void MainWindow::onBypassToggled(bool bypass) {
    m_audioEngine.setBypass(bypass);
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::DoubleClick) {
        if (isVisible()) {
            hide();
        } else {
            showNormal();
            activateWindow();
        }
    }
}

void MainWindow::onThemeChanged(const QString& themeName) {
    m_themeManager->applyTheme(themeName.toStdString(), this);
}

void MainWindow::onPresetSelected(const QString& presetName) {
    // Load preset
}

void MainWindow::updateAudioLevels(float left, float right) {
    if (m_analyzerView) {
        m_analyzerView->updateLevels(left, right);
    }
}

void MainWindow::updateSpectrum(const std::vector<float>& magnitudes) {
    if (m_homeView) {
        m_homeView->updateSpectrum(magnitudes);
    }
    if (m_analyzerView) {
        m_analyzerView->updateSpectrum(magnitudes);
    }
}

void MainWindow::showNotification(const QString& title, const QString& message) {
    if (m_trayIcon && m_trayIcon->isVisible()) {
        m_trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 3000);
    }
}

void MainWindow::minimizeToTray() {
    hide();
    if (m_trayIcon) {
        m_trayIcon->show();
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (m_minimizeToTray && m_trayIcon && m_trayIcon->isVisible()) {
        hide();
        event->ignore();
    } else {
        saveSettings();
        event->accept();
    }
}

void MainWindow::changeEvent(QEvent* event) {
    if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized() && m_minimizeToTray) {
            QTimer::singleShot(0, this, &MainWindow::hide);
        }
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::updateNavigationButtons(int index) {
    // Update button states
}

void MainWindow::handleSettingChange(const std::string& key, const QVariant& value) {
    LOG_DEBUG("Setting changed: {} = {}", key, value.toString().toStdString());
}

} // namespace SoundBoost
