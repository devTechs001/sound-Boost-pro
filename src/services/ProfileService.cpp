// src/services/ProfileService.cpp
#include "ProfileService.hpp"
#include "../utils/Logger.hpp"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStandardPaths>

namespace SoundBoost {

// AudioProfile serialization
QJsonObject AudioProfile::toJson() const {
    QJsonObject json;

    json["name"] = name;
    json["description"] = description;
    json["category"] = category;
    json["created"] = created.toString(Qt::ISODate);
    json["modified"] = modified.toString(Qt::ISODate);

    // Bass boost
    QJsonObject bass;
    bass["enabled"] = bassBoost.enabled;
    bass["frequency"] = bassBoost.frequency;
    bass["gain"] = bassBoost.gain;
    bass["width"] = bassBoost.width;
    json["bass_boost"] = bass;

    // Equalizer
    QJsonObject eq;
    eq["enabled"] = equalizer.enabled;
    QJsonArray bands;
    for (float gain : equalizer.bandGains) {
        bands.append(gain);
    }
    eq["band_gains"] = bands;
    json["equalizer"] = eq;

    // Compressor
    QJsonObject comp;
    comp["enabled"] = compressor.enabled;
    comp["threshold"] = compressor.threshold;
    comp["ratio"] = compressor.ratio;
    comp["attack"] = compressor.attack;
    comp["release"] = compressor.release;
    json["compressor"] = comp;

    // Stereo widener
    QJsonObject stereo;
    stereo["enabled"] = stereoWidener.enabled;
    stereo["width"] = stereoWidener.width;
    json["stereo_widener"] = stereo;

    // Reverb
    QJsonObject rev;
    rev["enabled"] = reverb.enabled;
    rev["room_size"] = reverb.roomSize;
    rev["damping"] = reverb.damping;
    rev["wet"] = reverb.wet;
    json["reverb"] = rev;

    json["adaptive_eq"] = adaptiveEQEnabled;

    return json;
}

AudioProfile AudioProfile::fromJson(const QJsonObject& json) {
    AudioProfile profile;

    profile.name = json["name"].toString();
    profile.description = json["description"].toString();
    profile.category = json["category"].toString();
    profile.created = QDateTime::fromString(json["created"].toString(), Qt::ISODate);
    profile.modified = QDateTime::fromString(json["modified"].toString(), Qt::ISODate);

    // Bass boost
    QJsonObject bass = json["bass_boost"].toObject();
    profile.bassBoost.enabled = bass["enabled"].toBool();
    profile.bassBoost.frequency = bass["frequency"].toDouble();
    profile.bassBoost.gain = bass["gain"].toDouble();
    profile.bassBoost.width = bass["width"].toDouble();

    // Equalizer
    QJsonObject eq = json["equalizer"].toObject();
    profile.equalizer.enabled = eq["enabled"].toBool();
    QJsonArray bands = eq["band_gains"].toArray();
    for (const auto& gain : bands) {
        profile.equalizer.bandGains.push_back(gain.toDouble());
    }

    // Compressor
    QJsonObject comp = json["compressor"].toObject();
    profile.compressor.enabled = comp["enabled"].toBool();
    profile.compressor.threshold = comp["threshold"].toDouble();
    profile.compressor.ratio = comp["ratio"].toDouble();
    profile.compressor.attack = comp["attack"].toDouble();
    profile.compressor.release = comp["release"].toDouble();

    // Stereo widener
    QJsonObject stereo = json["stereo_widener"].toObject();
    profile.stereoWidener.enabled = stereo["enabled"].toBool();
    profile.stereoWidener.width = stereo["width"].toDouble();

    // Reverb
    QJsonObject rev = json["reverb"].toObject();
    profile.reverb.enabled = rev["enabled"].toBool();
    profile.reverb.roomSize = rev["room_size"].toDouble();
    profile.reverb.damping = rev["damping"].toDouble();
    profile.reverb.wet = rev["wet"].toDouble();

    profile.adaptiveEQEnabled = json["adaptive_eq"].toBool();

    return profile;
}

// ProfileService implementation
ProfileService::ProfileService() = default;
ProfileService::~ProfileService() = default;

bool ProfileService::initialize() {
    // Set profiles directory
    QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    m_profilesDirectory = appData + "/profiles";

    // Create directory if needed
    QDir dir(m_profilesDirectory);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    loadProfiles();

    // Create defaults if no profiles exist
    if (m_profiles.empty()) {
        createDefaultProfiles();
    }

    LOG_INFO("ProfileService initialized with {} profiles", m_profiles.size());
    return true;
}

void ProfileService::shutdown() {
    saveProfiles();
}

void ProfileService::loadProfiles() {
    m_profiles.clear();

    QDir dir(m_profilesDirectory);
    QStringList filters;
    filters << "*.json";

    for (const QString& filename : dir.entryList(filters, QDir::Files)) {
        QString path = dir.filePath(filename);
        QFile file(path);

        if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            if (!doc.isNull() && doc.isObject()) {
                AudioProfile profile = AudioProfile::fromJson(doc.object());
                m_profiles.push_back(profile);
            }
        }
    }

    LOG_DEBUG("Loaded {} profiles", m_profiles.size());
}

void ProfileService::saveProfiles() {
    for (const auto& profile : m_profiles) {
        QString filename = profile.name.toLower().replace(" ", "_") + ".json";
        QString path = m_profilesDirectory + "/" + filename;

        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            QJsonDocument doc(profile.toJson());
            file.write(doc.toJson(QJsonDocument::Indented));
        }
    }

    LOG_DEBUG("Saved {} profiles", m_profiles.size());
}

std::vector<QString> ProfileService::getProfileNames() const {
    std::vector<QString> names;
    for (const auto& profile : m_profiles) {
        names.push_back(profile.name);
    }
    return names;
}

std::vector<AudioProfile> ProfileService::getAllProfiles() const {
    return m_profiles;
}

AudioProfile ProfileService::getProfile(const QString& name) const {
    for (const auto& profile : m_profiles) {
        if (profile.name == name) {
            return profile;
        }
    }
    return AudioProfile{};
}

bool ProfileService::hasProfile(const QString& name) const {
    for (const auto& profile : m_profiles) {
        if (profile.name == name) {
            return true;
        }
    }
    return false;
}

bool ProfileService::saveProfile(const AudioProfile& profile) {
    // Find and replace, or add new
    for (auto& p : m_profiles) {
        if (p.name == profile.name) {
            p = profile;
            p.modified = QDateTime::currentDateTime();
            saveProfiles();
            emit profileSaved(profile.name);
            return true;
        }
    }

    // Add new
    AudioProfile newProfile = profile;
    newProfile.created = QDateTime::currentDateTime();
    newProfile.modified = newProfile.created;
    m_profiles.push_back(newProfile);

    saveProfiles();
    emit profileSaved(profile.name);
    emit profilesChanged();

    return true;
}

bool ProfileService::deleteProfile(const QString& name) {
    for (auto it = m_profiles.begin(); it != m_profiles.end(); ++it) {
        if (it->name == name) {
            // Delete file
            QString filename = name.toLower().replace(" ", "_") + ".json";
            QFile::remove(m_profilesDirectory + "/" + filename);

            m_profiles.erase(it);
            emit profileDeleted(name);
            emit profilesChanged();
            return true;
        }
    }
    return false;
}

void ProfileService::loadProfile(const QString& name) {
    AudioProfile profile = getProfile(name);
    if (!profile.name.isEmpty()) {
        loadProfile(profile);
    }
}

void ProfileService::loadProfile(const AudioProfile& profile) {
    m_currentProfile = profile.name;

    // Apply settings to audio engine
    // This would be connected to the actual audio engine

    emit profileLoaded(profile.name);
    LOG_INFO("Loaded profile: {}", profile.name.toStdString());
}

void ProfileService::createDefaultProfiles() {
    // Bass Heavy
    AudioProfile bassHeavy;
    bassHeavy.name = "Bass Heavy";
    bassHeavy.description = "Deep, powerful bass enhancement";
    bassHeavy.category = "Music";
    bassHeavy.bassBoost = {true, 60.0f, 10.0f, 1.5f};
    bassHeavy.equalizer = {true, {8, 6, 4, 2, 0, 0, 0, 0, 0, 0}};
    bassHeavy.compressor = {false, -20.0f, 4.0f, 10.0f, 100.0f};
    bassHeavy.stereoWidener = {true, 120.0f};
    bassHeavy.reverb = {false, 0.5f, 0.5f, 0.2f};
    saveProfile(bassHeavy);

    // Vocal Boost
    AudioProfile vocalBoost;
    vocalBoost.name = "Vocal Boost";
    vocalBoost.description = "Enhanced clarity for vocals";
    vocalBoost.category = "Music";
    vocalBoost.bassBoost = {true, 80.0f, 3.0f, 1.0f};
    vocalBoost.equalizer = {true, {-2, -1, 0, 2, 4, 4, 2, 0, -1, -2}};
    vocalBoost.compressor = {true, -15.0f, 3.0f, 20.0f, 150.0f};
    vocalBoost.stereoWidener = {true, 100.0f};
    vocalBoost.reverb = {false, 0.3f, 0.6f, 0.1f};
    saveProfile(vocalBoost);

    // Flat
    AudioProfile flat;
    flat.name = "Flat";
    flat.description = "No enhancement, original sound";
    flat.category = "Reference";
    flat.bassBoost = {false, 80.0f, 0.0f, 1.0f};
    flat.equalizer = {true, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    flat.compressor = {false, -20.0f, 4.0f, 10.0f, 100.0f};
    flat.stereoWidener = {false, 100.0f};
    flat.reverb = {false, 0.5f, 0.5f, 0.0f};
    saveProfile(flat);

    // Gaming
    AudioProfile gaming;
    gaming.name = "Gaming";
    gaming.description = "Enhanced spatial awareness for gaming";
    gaming.category = "Gaming";
    gaming.bassBoost = {true, 100.0f, 4.0f, 1.2f};
    gaming.equalizer = {true, {4, 2, 0, -1, 0, 2, 4, 3, 2, 1}};
    gaming.compressor = {true, -12.0f, 2.0f, 5.0f, 50.0f};
    gaming.stereoWidener = {true, 150.0f};
    gaming.reverb = {false, 0.2f, 0.7f, 0.1f};
    saveProfile(gaming);

    // Movies
    AudioProfile movies;
    movies.name = "Movies";
    movies.description = "Cinematic sound for movies";
    movies.category = "Movies";
    movies.bassBoost = {true, 50.0f, 6.0f, 1.5f};
    movies.equalizer = {true, {6, 4, 2, 0, -1, 0, 1, 2, 3, 4}};
    movies.compressor = {true, -18.0f, 3.0f, 15.0f, 200.0f};
    movies.stereoWidener = {true, 130.0f};
    movies.reverb = {true, 0.6f, 0.4f, 0.15f};
    saveProfile(movies);

    LOG_INFO("Created {} default profiles", m_profiles.size());
}

bool ProfileService::exportProfile(const QString& name, const QString& path) const {
    AudioProfile profile = getProfile(name);
    if (profile.name.isEmpty()) {
        return false;
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonDocument doc(profile.toJson());
    file.write(doc.toJson(QJsonDocument::Indented));

    LOG_INFO("Exported profile '{}' to {}", name.toStdString(), path.toStdString());
    return true;
}

bool ProfileService::importProfile(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull() || !doc.isObject()) {
        return false;
    }

    AudioProfile profile = AudioProfile::fromJson(doc.object());

    // Ensure unique name
    QString baseName = profile.name;
    int counter = 1;
    while (hasProfile(profile.name)) {
        profile.name = QString("%1 (%2)").arg(baseName).arg(counter++);
    }

    return saveProfile(profile);
}

} // namespace SoundBoost
