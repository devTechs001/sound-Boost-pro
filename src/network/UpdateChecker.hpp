// src/network/UpdateChecker.hpp
#pragma once

#include <string>
#include <functional>

namespace SoundBoost {

class UpdateChecker {
public:
    using UpdateCallback = std::function<void(bool available, const std::string& version)>;

    static UpdateChecker& getInstance();

    void checkForUpdates(UpdateCallback callback);
    void downloadUpdate(const std::string& url);

private:
    UpdateChecker() = default;
};

} // namespace SoundBoost
