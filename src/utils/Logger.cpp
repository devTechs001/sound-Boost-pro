// src/utils/Logger.cpp
#include "Logger.hpp"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <filesystem>
#include <format>

namespace SoundBoost {

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::~Logger() {
    shutdown();
}

void Logger::initialize(LogLevel level, const std::string& logFile) {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_level = level;

    if (!logFile.empty()) {
        setFileOutput(true, logFile);
    }

    m_initialized = true;

    info("Logger initialized at level {}", levelToString(level));
}

void Logger::shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_fileStream.is_open()) {
        m_fileStream.close();
    }

    m_initialized = false;
}

void Logger::setFileOutput(bool enabled, const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_fileStream.is_open()) {
        m_fileStream.close();
    }

    m_fileOutput = enabled;

    if (enabled && !path.empty()) {
        // Create directory if needed
        std::filesystem::path filePath(path);
        if (filePath.has_parent_path()) {
            std::filesystem::create_directories(filePath.parent_path());
        }

        m_fileStream.open(path, std::ios::out | std::ios::app);
        if (!m_fileStream.is_open()) {
            std::cerr << "Failed to open log file: " << path << std::endl;
            m_fileOutput = false;
        }
    }
}

template<typename... Args>
std::string Logger::formatMessage(std::string_view format, Args&&... args) {
    try {
        return std::vformat(format, std::make_format_args(args...));
    } catch (const std::format_error& e) {
        return std::string(format) + " [FORMAT ERROR: " + e.what() + "]";
    }
}

void Logger::writeLog(LogLevel level, const std::source_location& loc,
                      const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::string timestamp = getTimestamp();
    std::string levelStr = levelToString(level);

    // Extract filename from path
    std::string_view filename = loc.file_name();
    auto lastSlash = filename.find_last_of("/\\");
    if (lastSlash != std::string_view::npos) {
        filename = filename.substr(lastSlash + 1);
    }

    std::ostringstream formatted;
    formatted << "[" << timestamp << "] "
              << "[" << levelStr << "] "
              << "[" << filename << ":" << loc.line() << "] "
              << message;

    std::string logLine = formatted.str();

    // Console output
    if (m_consoleOutput) {
        std::string color = levelToColor(level);
        std::cout << color << logLine << "\033[0m" << std::endl;
    }

    // File output
    if (m_fileOutput && m_fileStream.is_open()) {
        m_fileStream << logLine << std::endl;
        m_fileStream.flush();
    }
}

std::string Logger::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Trace:    return "TRACE";
        case LogLevel::Debug:    return "DEBUG";
        case LogLevel::Info:     return "INFO ";
        case LogLevel::Warning:  return "WARN ";
        case LogLevel::Error:    return "ERROR";
        case LogLevel::Critical: return "CRIT ";
        default:                 return "?????";
    }
}

std::string Logger::levelToColor(LogLevel level) {
    switch (level) {
        case LogLevel::Trace:    return "\033[90m";      // Gray
        case LogLevel::Debug:    return "\033[36m";      // Cyan
        case LogLevel::Info:     return "\033[32m";      // Green
        case LogLevel::Warning:  return "\033[33m";      // Yellow
        case LogLevel::Error:    return "\033[31m";      // Red
        case LogLevel::Critical: return "\033[1;31m";    // Bold Red
        default:                 return "\033[0m";
    }
}

// Explicit template instantiations for common types
template std::string Logger::formatMessage(std::string_view, const char*);
template std::string Logger::formatMessage(std::string_view, std::string);
template std::string Logger::formatMessage(std::string_view, int);
template std::string Logger::formatMessage(std::string_view, float);
template std::string Logger::formatMessage(std::string_view, int, int);
template std::string Logger::formatMessage(std::string_view, const char*, int);

} // namespace SoundBoost
