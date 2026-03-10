// src/network/TelemetryClient.hpp
#pragma once

#include <string>
#include <unordered_map>

namespace SoundBoost {

class TelemetryClient {
public:
    static TelemetryClient& getInstance();

    void initialize(const std::string& endpoint);
    void trackEvent(const std::string& eventName);
    void trackMetric(const std::string& name, double value);

private:
    TelemetryClient() = default;
    std::string m_endpoint;
};

} // namespace SoundBoost
