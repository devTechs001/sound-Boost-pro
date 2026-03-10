// src/network/CloudSync.hpp
#pragma once

#include <string>
#include <functional>

namespace SoundBoost {

class CloudSync {
public:
    using SyncCallback = std::function<void(bool success)>;

    static CloudSync& getInstance();

    void initialize(const std::string& apiKey);
    void uploadSettings(const std::string& data, SyncCallback callback);
    void downloadSettings(SyncCallback callback);

private:
    CloudSync() = default;
    std::string m_apiKey;
};

} // namespace SoundBoost
