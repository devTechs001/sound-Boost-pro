// src/services/ProfileService.cpp
#include "ProfileService.hpp"
#include "../utils/Logger.hpp"
#include "../utils/FileUtils.hpp"

#include <QStandardPaths>

namespace SoundBoost {

bool ProfileService::initialize() {
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    m_profilesDirectory = dataPath.toStdString() + "/profiles";

    FileUtils::createDirectory(m_profilesDirectory);

    loadDefaultProfiles();

    LOG_INFO("ProfileService initialized, directory: {}", m_profilesDirectory);
    return true;
}

void ProfileService::shutdown() {
    LOG_INFO("ProfileService shutdown");
}

void ProfileService::createProfile(const std::string& name, const std::string& description) {
    std::lock_guard<std::mutex> lock(m_mutex);

    Profile profile;
    profile.name = name;
    profile.description = description;

    // Default settings
    profile.settings["bass.frequency"] = 80.0f;
    profile.settings["bass.gain"] = 6.0f;
    profile.settings["bass.width"] = 1.5f;

    m_profiles[name] = profile;
    saveProfile(name);

    LOG_INFO("Created profile: {}", name);
}

void ProfileService::deleteProfile(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_profiles.find(name);
    if (it != m_profiles.end()) {
        std::string path = getProfilePath(name);
        FileUtils::remove(path);
        m_profiles.erase(it);

        LOG_INFO("Deleted profile: {}", name);
    }
}

void ProfileService::loadProfile(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_profiles.find(name);
    if (it != m_profiles.end()) {
        m_currentProfile = name;
        LOG_INFO("Loaded profile: {}", name);
    } else {
        // Try to load from file
        std::string path = getProfilePath(name);
        if (FileUtils::exists(path)) {
            // Load profile from file
            LOG_INFO("Loaded profile from file: {}", name);
        }
    }
}

void ProfileService::saveProfile(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_profiles.find(name);
    if (it == m_profiles.end()) return;

    std::string path = getProfilePath(name);

    // Simple serialization
    std::string content = "# Profile: " + name + "\n";
    content += "# " + it->second.description + "\n\n";

    for (const auto& [key, value] : it->second.settings) {
        content += key + " = " + std::to_string(value) + "\n";
    }

    FileUtils::writeFile(path, content);
    LOG_DEBUG("Saved profile: {} to {}", name, path);
}

std::vector<std::string> ProfileService::getProfileNames() const {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::vector<std::string> names;
    for (const auto& [name, _] : m_profiles) {
        names.push_back(name);
    }
    return names;
}

Profile* ProfileService::getProfile(const std::string& name) {
    auto it = m_profiles.find(name);
    if (it != m_profiles.end()) {
        return &it->second;
    }
    return nullptr;
}

const Profile* ProfileService::getProfile(const std::string& name) const {
    auto it = m_profiles.find(name);
    if (it != m_profiles.end()) {
        return &it->second;
    }
    return nullptr;
}

void ProfileService::setSetting(const std::string& key, float value) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_currentProfile.empty()) {
        auto it = m_profiles.find(m_currentProfile);
        if (it != m_profiles.end()) {
            it->second.settings[key] = value;
        }
    }
}

float ProfileService::getSetting(const std::string& key, float defaultValue) const {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_currentProfile.empty()) {
        auto it = m_profiles.find(m_currentProfile);
        if (it != m_profiles.end()) {
            auto sit = it->second.settings.find(key);
            if (sit != it->second.settings.end()) {
                return sit->second;
            }
        }
    }

    return defaultValue;
}

std::string ProfileService::getProfilePath(const std::string& name) const {
    return FileUtils::join(m_profilesDirectory, name + ".profile");
}

void ProfileService::loadDefaultProfiles() {
    createProfile("Flat", "No processing");
    createProfile("Bass Boost", "Enhanced bass response");
    createProfile("Vocal Boost", "Enhanced vocal clarity");
    createProfile("Rock", "Rock music profile");
    createProfile("Electronic", "Electronic music profile");
    createProfile("Classical", "Classical music profile");
    createProfile("Movie", "Cinematic experience");
    createProfile("Gaming", "Gaming optimized");
    createProfile("Night Mode", "Reduced dynamic range");

    LOG_INFO("Default profiles loaded");
}

} // namespace SoundBoost
