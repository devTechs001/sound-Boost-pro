// src/services/SettingsService.cpp
#include "SettingsService.hpp"
#include "../utils/Logger.hpp"

#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

namespace SoundBoost {

SettingsService::SettingsService() = default;
SettingsService::~SettingsService() = default;

bool SettingsService::initialize() {
    m_settings = std::make_unique<QSettings>(
        QSettings::IniFormat,
        QSettings::UserScope,
        QCoreApplication::organizationName(),
        QCoreApplication::applicationName()
    );

    // Set defaults if first run
    if (!m_settings->contains("initialized")) {
        setDefaults();
        m_settings->setValue("initialized", true);
    }

    LOG_INFO("Settings loaded from: {}",
        m_settings->fileName().toStdString());

    return true;
}

void SettingsService::shutdown() {
    save();
}

void SettingsService::setDefaults() {
    // Audio settings
    m_settings->setValue("audio/sample_rate", 48000);
    m_settings->setValue("audio/buffer_size", 512);
    m_settings->setValue("audio/channels", 2);
    m_settings->setValue("audio/bit_depth", 32);
    m_settings->setValue("audio/master_volume", 100);
    m_settings->setValue("audio/master_enabled", true);

    // UI settings
    m_settings->setValue("ui/theme", "dark");
    m_settings->setValue("ui/minimize_to_tray", true);
    m_settings->setValue("ui/start_minimized", false);
    m_settings->setValue("ui/show_spectrum", true);
    m_settings->setValue("ui/language", "en");

    // ML settings
    m_settings->setValue("ml/enabled", true);
    m_settings->setValue("ml/adaptive_eq", false);
    m_settings->setValue("ml/genre_detection", true);

    // Effects defaults
    m_settings->setValue("effects/bass_boost/enabled", true);
    m_settings->setValue("effects/bass_boost/frequency", 80.0f);
    m_settings->setValue("effects/bass_boost/gain", 6.0f);

    m_settings->setValue("effects/equalizer/enabled", true);
    m_settings->setValue("effects/equalizer/preset", "Flat");

    m_settings->setValue("effects/compressor/enabled", false);
    m_settings->setValue("effects/compressor/threshold", -20.0f);
    m_settings->setValue("effects/compressor/ratio", 4.0f);

    m_settings->sync();
}

QVariant SettingsService::get(const QString& key, const QVariant& defaultValue) const {
    return m_settings->value(key, defaultValue);
}

void SettingsService::set(const QString& key, const QVariant& value) {
    m_settings->setValue(key, value);
    emit settingChanged(key, value);
}

void SettingsService::remove(const QString& key) {
    m_settings->remove(key);
}

bool SettingsService::contains(const QString& key) const {
    return m_settings->contains(key);
}

void SettingsService::beginGroup(const QString& prefix) {
    m_settings->beginGroup(prefix);
}

void SettingsService::endGroup() {
    m_settings->endGroup();
}

QStringList SettingsService::childKeys() const {
    return m_settings->childKeys();
}

QStringList SettingsService::childGroups() const {
    return m_settings->childGroups();
}

void SettingsService::save() {
    m_settings->sync();
    LOG_DEBUG("Settings saved");
}

void SettingsService::load() {
    m_settings->sync();
    LOG_DEBUG("Settings reloaded");
}

void SettingsService::reset() {
    m_settings->clear();
    setDefaults();
    emit settingsReset();
    LOG_INFO("Settings reset to defaults");
}

bool SettingsService::exportToFile(const QString& path) const {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        LOG_ERROR("Failed to open file for export: {}", path.toStdString());
        return false;
    }

    QJsonObject root;

    for (const QString& key : m_settings->allKeys()) {
        QVariant value = m_settings->value(key);
        root[key] = QJsonValue::fromVariant(value);
    }

    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));

    LOG_INFO("Settings exported to: {}", path.toStdString());
    return true;
}

bool SettingsService::importFromFile(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        LOG_ERROR("Failed to open file for import: {}", path.toStdString());
        return false;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);

    if (error.error != QJsonParseError::NoError) {
        LOG_ERROR("Failed to parse settings file: {}", error.errorString().toStdString());
        return false;
    }

    QJsonObject root = doc.object();

    for (auto it = root.begin(); it != root.end(); ++it) {
        m_settings->setValue(it.key(), it.value().toVariant());
    }

    m_settings->sync();

    LOG_INFO("Settings imported from: {}", path.toStdString());
    return true;
}

} // namespace SoundBoost
