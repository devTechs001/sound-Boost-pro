// src/utils/Configuration.cpp
#include "Configuration.hpp"
#include "Logger.hpp"
#include <fstream>
#include <sstream>

namespace SoundBoost {

Configuration& Configuration::getInstance() {
    static Configuration instance;
    return instance;
}

void Configuration::load(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::string configPath = path;
    if (configPath.empty()) {
        configPath = "config.ini";
    }

    std::ifstream file(configPath);
    if (!file.is_open()) {
        LOG_INFO("No config file found, using defaults");
        return;
    }

    m_configPath = configPath;
    std::string line;

    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;

        // Parse key=value
        auto pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            // Try to parse as different types
            try {
                if (value == "true" || value == "false") {
                    m_values[key] = (value == "true");
                } else if (value.find('.') != std::string::npos) {
                    m_values[key] = std::stof(value);
                } else {
                    m_values[key] = std::stoi(value);
                }
            } catch (...) {
                m_values[key] = value;
            }
        }
    }

    LOG_INFO("Configuration loaded from {}", configPath);
}

void Configuration::save(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::string configPath = path;
    if (configPath.empty()) {
        configPath = m_configPath.empty() ? "config.ini" : m_configPath;
    }

    std::ofstream file(configPath);
    if (!file.is_open()) {
        LOG_ERROR("Failed to save configuration to {}", configPath);
        return;
    }

    file << "# SoundBoost Pro Configuration\n\n";

    for (const auto& [key, value] : m_values) {
        file << key << " = ";

        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << val;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else {
                file << val;
            }
        }, value);

        file << "\n";
    }

    LOG_INFO("Configuration saved to {}", configPath);
}

void Configuration::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_values.clear();
}

} // namespace SoundBoost
