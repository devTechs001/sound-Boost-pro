// src/services/ProfileService.hpp
#pragma once

#include "ServiceLocator.hpp"
#include <QObject>
#include <QString>
#include <vector>
#include <memory>
#include <QDateTime>
#include <QJsonObject>

namespace SoundBoost {

struct AudioProfile {
    QString name;
    QString description;
    QString category;
    QDateTime created;
    QDateTime modified;

    // Effect settings
    struct {
        bool enabled;
        float frequency;
        float gain;
        float width;
    } bassBoost;

    struct {
        bool enabled;
        std::vector<float> bandGains;
    } equalizer;

    struct {
        bool enabled;
        float threshold;
        float ratio;
        float attack;
        float release;
    } compressor;

    struct {
        bool enabled;
        float width;
    } stereoWidener;

    struct {
        bool enabled;
        float roomSize;
        float damping;
        float wet;
    } reverb;

    // ML settings
    bool adaptiveEQEnabled;

    // Serialize/deserialize
    QJsonObject toJson() const;
    static AudioProfile fromJson(const QJsonObject& json);
};

class ProfileService : public QObject, public IService {
    Q_OBJECT

public:
    ProfileService();
    ~ProfileService() override;

    bool initialize() override;
    void shutdown() override;
    std::string getName() const override { return "ProfileService"; }

    // Profile management
    std::vector<QString> getProfileNames() const;
    std::vector<AudioProfile> getAllProfiles() const;

    AudioProfile getProfile(const QString& name) const;
    bool hasProfile(const QString& name) const;

    bool saveProfile(const AudioProfile& profile);
    bool deleteProfile(const QString& name);
    bool renameProfile(const QString& oldName, const QString& newName);
    bool duplicateProfile(const QString& name, const QString& newName);

    // Current profile
    void loadProfile(const QString& name);
    void loadProfile(const AudioProfile& profile);
    QString getCurrentProfileName() const { return m_currentProfile; }
    AudioProfile getCurrentProfile() const;

    // Capture current settings as profile
    AudioProfile captureCurrentSettings(const QString& name);

    // Import/Export
    bool exportProfile(const QString& name, const QString& path) const;
    bool importProfile(const QString& path);
    bool exportAllProfiles(const QString& directory) const;

    // Default profiles
    void createDefaultProfiles();

signals:
    void profileLoaded(const QString& name);
    void profileSaved(const QString& name);
    void profileDeleted(const QString& name);
    void profilesChanged();

private:
    void loadProfiles();
    void saveProfiles();
    QString getProfilesPath() const;

    std::vector<AudioProfile> m_profiles;
    QString m_currentProfile;
    QString m_profilesDirectory;
};

} // namespace SoundBoost
