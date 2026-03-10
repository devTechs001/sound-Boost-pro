// src/services/SettingsService.hpp
#pragma once

#include "ServiceLocator.hpp"
#include <QObject>
#include <QSettings>
#include <QVariant>
#include <memory>
#include <functional>

namespace SoundBoost {

class SettingsService : public QObject, public IService {
    Q_OBJECT

public:
    SettingsService();
    ~SettingsService() override;

    bool initialize() override;
    void shutdown() override;
    std::string getName() const override { return "SettingsService"; }

    // Getters
    template<typename T>
    T get(const QString& key, const T& defaultValue = T{}) const {
        QVariant value = m_settings->value(key, QVariant::fromValue(defaultValue));
        return value.value<T>();
    }

    QVariant get(const QString& key, const QVariant& defaultValue = QVariant()) const;

    // Setters
    template<typename T>
    void set(const QString& key, const T& value) {
        m_settings->setValue(key, QVariant::fromValue(value));
        emit settingChanged(key, QVariant::fromValue(value));
    }

    void set(const QString& key, const QVariant& value);

    // Remove
    void remove(const QString& key);
    bool contains(const QString& key) const;

    // Groups
    void beginGroup(const QString& prefix);
    void endGroup();
    QStringList childKeys() const;
    QStringList childGroups() const;

    // Persistence
    void save();
    void load();
    void reset();

    // Export/Import
    bool exportToFile(const QString& path) const;
    bool importFromFile(const QString& path);

signals:
    void settingChanged(const QString& key, const QVariant& value);
    void settingsReset();

private:
    void setDefaults();

    std::unique_ptr<QSettings> m_settings;
};

} // namespace SoundBoost
