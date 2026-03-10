// src/utils/Logger.hpp
#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <fstream>
#include <sstream>
#include <chrono>

namespace SoundBoost {

enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5,
    Off = 6
};

class Logger {
public:
    static Logger& getInstance();

    void initialize(LogLevel level = LogLevel::Info,
                   const std::string& logFile = "");
    void shutdown();

    void setLevel(LogLevel level) { m_level = level; }
    LogLevel getLevel() const { return m_level; }

    void setConsoleOutput(bool enabled) { m_consoleOutput = enabled; }
    void setFileOutput(bool enabled, const std::string& path = "");

    void log(LogLevel level, const char* file, int line, const std::string& message);

    void trace(const std::string& msg) { log(LogLevel::Trace, "", 0, msg); }
    void debug(const std::string& msg) { log(LogLevel::Debug, "", 0, msg); }
    void info(const std::string& msg) { log(LogLevel::Info, "", 0, msg); }
    void warning(const std::string& msg) { log(LogLevel::Warning, "", 0, msg); }
    void error(const std::string& msg) { log(LogLevel::Error, "", 0, msg); }
    void critical(const std::string& msg) { log(LogLevel::Critical, "", 0, msg); }

private:
    Logger() = default;
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void writeLog(LogLevel level, const char* file, int line, const std::string& message);
    std::string getTimestamp();
    std::string levelToString(LogLevel level);

    LogLevel m_level{LogLevel::Info};
    bool m_consoleOutput{true};
    bool m_fileOutput{false};

    std::ofstream m_fileStream;
    std::mutex m_mutex;

    bool m_initialized{false};
};

// Convenience macros
#define LOG_TRACE(msg) SoundBoost::Logger::getInstance().log(LogLevel::Trace, __FILE__, __LINE__, msg)
#define LOG_DEBUG(msg) SoundBoost::Logger::getInstance().log(LogLevel::Debug, __FILE__, __LINE__, msg)
#define LOG_INFO(msg) SoundBoost::Logger::getInstance().log(LogLevel::Info, __FILE__, __LINE__, msg)
#define LOG_WARN(msg) SoundBoost::Logger::getInstance().log(LogLevel::Warning, __FILE__, __LINE__, msg)
#define LOG_ERROR(msg) SoundBoost::Logger::getInstance().log(LogLevel::Error, __FILE__, __LINE__, msg)
#define LOG_CRITICAL(msg) SoundBoost::Logger::getInstance().log(LogLevel::Critical, __FILE__, __LINE__, msg)

} // namespace SoundBoost
