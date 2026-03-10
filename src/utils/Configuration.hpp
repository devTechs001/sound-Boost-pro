// src/utils/Configuration.hpp
#pragma once

#include <string>
#include <variant>
#include <map>
#include <mutex>
#include <optional>

namespace SoundBoost {

using ConfigValue = std::variant<
    bool, int, float, double, std::string
>;

class Configuration {
public:
    static Configuration& getInstance();

    bool load(const std::string& path = "");
    bool save(const std::string& path = "");
    bool reload();

    // Getters
    template<typename T>
    T get(const std::string& key, const T& defaultValue = T{}) const;

    template<typename T>
    std::optional<T> getOptional(const std::string& key) const;

    bool has(const std::string& key) const;

    // Setters
    template<typename T>
    void set(const std::string& key, const T& value);

    void remove(const std::string& key);
    void clear();

    // Path management
    std::string getConfigPath() const { return m_configPath; }
    void setConfigPath(const std::string& path) { m_configPath = path; }

    // Default config paths
    static std::string getDefaultConfigPath();
    static std::string getUserConfigPath();

private:
    Configuration() = default;
    ~Configuration() = default;

    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;

    std::map<std::string, ConfigValue> m_config;
    std::string m_configPath;
    mutable std::mutex m_mutex;
};

// Template implementations
template<typename T>
T Configuration::get(const std::string& key, const T& defaultValue) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_config.find(key);
    if (it != m_config.end()) {
        try {
            return std::get<T>(it->second);
        } catch (const std::bad_variant_access&) {
            return defaultValue;
        }
    }
    return defaultValue;
}

template<typename T>
std::optional<T> Configuration::getOptional(const std::string& key) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_config.find(key);
    if (it != m_config.end()) {
        try {
            return std::get<T>(it->second);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    return std::nullopt;
}

template<typename T>
void Configuration::set(const std::string& key, const T& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config[key] = value;
}

} // namespace SoundBoost
