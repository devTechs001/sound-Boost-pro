// src/services/ProfileService.hpp
#pragma once

#include "ServiceLocator.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

namespace SoundBoost {

struct Profile {
    std::string name;
    std::string description;
    std::unordered_map<std::string, float> settings;
};

class ProfileService : public IService {
public:
    ProfileService() = default;
    ~ProfileService() override = default;

    std::string getName() const override { return "ProfileService"; }

    bool initialize() override;
    void shutdown() override;

    // Profile management
    void createProfile(const std::string& name, const std::string& description = "");
    void deleteProfile(const std::string& name);
    void loadProfile(const std::string& name);
    void saveProfile(const std::string& name);

    std::vector<std::string> getProfileNames() const;
    Profile* getProfile(const std::string& name);
    const Profile* getProfile(const std::string& name) const;

    std::string getCurrentProfile() const { return m_currentProfile; }

    void setSetting(const std::string& key, float value);
    float getSetting(const std::string& key, float defaultValue = 0.0f) const;

private:
    std::string getProfilePath(const std::string& name) const;
    void loadDefaultProfiles();

    std::unordered_map<std::string, Profile> m_profiles;
    std::string m_currentProfile;
    std::string m_profilesDirectory;
    mutable std::mutex m_mutex;
};

} // namespace SoundBoost
