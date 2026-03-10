// src/services/SettingsService.cpp
#include "SettingsService.hpp"
#include "../utils/Logger.hpp"
#include "../utils/FileUtils.hpp"

#include <QSettings>
#include <QStandardPaths>

namespace SoundBoost {

bool SettingsService::initialize() {
    // Get settings path
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    m_settingsPath = configPath.toStdString() + "/SoundBoostPro/settings.ini";

    LOG_INFO("SettingsService initialized, path: {}", m_settingsPath);

    load();
    return true;
}

void SettingsService::shutdown() {
    save();
    LOG_INFO("SettingsService shutdown");
}

void SettingsService::save() {
    std::lock_guard<std::mutex> lock(m_mutex);

    QSettings settings(QString::fromStdString(m_settingsPath), QSettings::IniFormat);

    for (const auto& [key, value] : m_settings) {
        settings.setValue(QString::fromStdString(key), value);
    }

    settings.sync();
    LOG_DEBUG("Settings saved");
}

void SettingsService::load() {
    std::lock_guard<std::mutex> lock(m_mutex);

    QSettings settings(QString::fromStdString(m_settingsPath), QSettings::IniFormat);

    for (const QString& key : settings.allKeys()) {
        m_settings[key.toStdString()] = settings.value(key);
    }

    LOG_DEBUG("Settings loaded");
}

void SettingsService::reset() {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_settings.clear();

    // Set defaults
    m_settings["audio.sample_rate"] = 48000;
    m_settings["audio.buffer_size"] = 512;
    m_settings["audio.channels"] = 2;
    m_settings["audio.master_volume"] = 100;
    m_settings["audio.master_enabled"] = true;

    m_settings["ui.theme"] = "dark";
    m_settings["ui.minimize_to_tray"] = true;

    m_settings["ml.model_path"] = "models/";
    m_settings["ml.adaptive_enabled"] = true;

    LOG_INFO("Settings reset to defaults");
}

} // namespace SoundBoost
