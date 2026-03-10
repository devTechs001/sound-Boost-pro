// src/services/SettingsService.hpp
#pragma once

#include "ServiceLocator.hpp"
#include <QVariant>
#include <string>
#include <unordered_map>
#include <mutex>

namespace SoundBoost {

class SettingsService : public IService {
    Q_OBJECT

public:
    SettingsService() = default;
    ~SettingsService() override = default;

    std::string getName() const override { return "SettingsService"; }

    bool initialize() override;
    void shutdown() override;

    // Settings access
    template<typename T>
    T get(const std::string& key, T defaultValue) const;

    template<typename T>
    void set(const std::string& key, const T& value);

    void save();
    void load();

    void reset();

signals:
    void settingChanged(const QString& key, const QVariant& value);

private:
    std::unordered_map<std::string, QVariant> m_settings;
    mutable std::mutex m_mutex;
    std::string m_settingsPath;
};

// Template implementations
template<typename T>
T SettingsService::get(const std::string& key, T defaultValue) const {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_settings.find(key);
    if (it != m_settings.end()) {
        bool ok = false;
        T value = it->second.value<T>(&ok);
        if (ok) {
            return value;
        }
    }
    return defaultValue;
}

template<typename T>
void SettingsService::set(const std::string& key, const T& value) {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_settings[key] = QVariant::fromValue(value);

    emit settingChanged(QString::fromStdString(key), QVariant::fromValue(value));
}

} // namespace SoundBoost
