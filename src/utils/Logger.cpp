// src/utils/Logger.cpp
#include "Logger.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace SoundBoost {

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::initialize(LogLevel level, const std::string& logFile) {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_level = level;

    if (!logFile.empty()) {
        m_fileStream.open(logFile, std::ios::app);
    }

    m_initialized = true;
}

void Logger::log(LogLevel level, const char* file, int line, const std::string& message) {
    if (level < m_level || !m_initialized) return;

    std::lock_guard<std::mutex> lock(m_mutex);

    std::string formatted = formatMessage(level, file, line, message);

    // Output to console
    std::cout << formatted << std::endl;

    // Output to file if open
    if (m_fileStream.is_open()) {
        m_fileStream << formatted << std::endl;
    }
}

const char* Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace:    return "TRACE";
        case LogLevel::Debug:    return "DEBUG";
        case LogLevel::Info:     return "INFO";
        case LogLevel::Warning:  return "WARN";
        case LogLevel::Error:    return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}

std::string Logger::formatMessage(LogLevel level, const char* file, int line, const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    oss << " [" << std::setw(8) << levelToString(level) << "] ";
    oss << message;

    return oss.str();
}

} // namespace SoundBoost
