// src/utils/Configuration.hpp
#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <mutex>

namespace SoundBoost {

using ConfigValue = std::variant<int, float, double, std::string, bool>;

class Configuration {
public:
    static Configuration& getInstance();

    void load(const std::string& path = "");
    void save(const std::string& path = "");

    template<typename T>
    T get(const std::string& key, T defaultValue) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_values.find(key);
        if (it != m_values.end()) {
            try {
                return std::get<T>(it->second);
            } catch (...) {
                return defaultValue;
            }
        }
        return defaultValue;
    }

    template<typename T>
    void set(const std::string& key, T value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_values[key] = value;
    }

    void clear();

private:
    Configuration() = default;
    ~Configuration() = default;
    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;

    std::unordered_map<std::string, ConfigValue> m_values;
    std::string m_configPath;
    mutable std::mutex m_mutex;
};

} // namespace SoundBoost
