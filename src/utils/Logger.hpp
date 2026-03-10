// src/utils/Logger.hpp
#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <fstream>
#include <sstream>
#include <chrono>
#include <source_location>

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

    template<typename... Args>
    void log(LogLevel level, const std::source_location& loc,
             std::string_view format, Args&&... args) {
        if (level < m_level) return;

        std::string message = formatMessage(format, std::forward<Args>(args)...);
        writeLog(level, loc, message);
    }

    template<typename... Args>
    void trace(std::string_view format, Args&&... args) {
        log(LogLevel::Trace, std::source_location::current(), format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void debug(std::string_view format, Args&&... args) {
        log(LogLevel::Debug, std::source_location::current(), format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void info(std::string_view format, Args&&... args) {
        log(LogLevel::Info, std::source_location::current(), format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warning(std::string_view format, Args&&... args) {
        log(LogLevel::Warning, std::source_location::current(), format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(std::string_view format, Args&&... args) {
        log(LogLevel::Error, std::source_location::current(), format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void critical(std::string_view format, Args&&... args) {
        log(LogLevel::Critical, std::source_location::current(), format, std::forward<Args>(args)...);
    }

private:
    Logger() = default;
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    template<typename... Args>
    std::string formatMessage(std::string_view format, Args&&... args);

    void writeLog(LogLevel level, const std::source_location& loc,
                  const std::string& message);
    std::string getTimestamp();
    std::string levelToString(LogLevel level);
    std::string levelToColor(LogLevel level);

    LogLevel m_level{LogLevel::Info};
    bool m_consoleOutput{true};
    bool m_fileOutput{false};

    std::ofstream m_fileStream;
    std::mutex m_mutex;

    bool m_initialized{false};
};

// Convenience macros
#define LOG_TRACE(...) SoundBoost::Logger::getInstance().trace(__VA_ARGS__)
#define LOG_DEBUG(...) SoundBoost::Logger::getInstance().debug(__VA_ARGS__)
#define LOG_INFO(...) SoundBoost::Logger::getInstance().info(__VA_ARGS__)
#define LOG_WARN(...) SoundBoost::Logger::getInstance().warning(__VA_ARGS__)
#define LOG_ERROR(...) SoundBoost::Logger::getInstance().error(__VA_ARGS__)
#define LOG_CRITICAL(...) SoundBoost::Logger::getInstance().critical(__VA_ARGS__)

} // namespace SoundBoost
