// src/utils/Logger.hpp
#pragma once

#include <string>
#include <memory>
#include <fstream>
#include <mutex>
#include <iostream>

namespace SoundBoost {

enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

class Logger {
public:
    static Logger& getInstance();

    void initialize(LogLevel level = LogLevel::Info, const std::string& logFile = "");
    void setLevel(LogLevel level) { m_level = level; }

    void log(LogLevel level, const char* file, int line, const std::string& message);

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    const char* levelToString(LogLevel level) const;
    std::string formatMessage(LogLevel level, const char* file, int line, const std::string& message);

    LogLevel m_level{LogLevel::Info};
    std::ofstream m_fileStream;
    std::mutex m_mutex;
    bool m_initialized{false};
};

// Logging macros
#define LOG_TRACE(msg) SoundBoost::Logger::getInstance().log(SoundBoost::LogLevel::Trace, __FILE__, __LINE__, msg)
#define LOG_DEBUG(msg) SoundBoost::Logger::getInstance().log(SoundBoost::LogLevel::Debug, __FILE__, __LINE__, msg)
#define LOG_INFO(msg) SoundBoost::Logger::getInstance().log(SoundBoost::LogLevel::Info, __FILE__, __LINE__, msg)
#define LOG_WARN(msg) SoundBoost::Logger::getInstance().log(SoundBoost::LogLevel::Warning, __FILE__, __LINE__, msg)
#define LOG_ERROR(msg) SoundBoost::Logger::getInstance().log(SoundBoost::LogLevel::Error, __FILE__, __LINE__, msg)
#define LOG_CRITICAL(msg) SoundBoost::Logger::getInstance().log(SoundBoost::LogLevel::Critical, __FILE__, __LINE__, msg)

} // namespace SoundBoost
