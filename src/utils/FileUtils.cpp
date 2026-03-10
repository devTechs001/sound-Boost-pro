// src/utils/FileUtils.cpp
#include "FileUtils.hpp"
#include "Logger.hpp"
#include <fstream>
#include <sstream>
#include <regex>

namespace SoundBoost {

std::string FileUtils::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open file: {}", path);
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void FileUtils::writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        LOG_ERROR("Failed to create file: {}", path);
        return;
    }
    file << content;
}

std::vector<std::string> FileUtils::readLines(const std::string& path) {
    std::vector<std::string> lines;
    std::ifstream file(path);

    if (!file.is_open()) {
        LOG_ERROR("Failed to open file: {}", path);
        return lines;
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}

bool FileUtils::exists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool FileUtils::isDirectory(const std::string& path) {
    return std::filesystem::is_directory(path);
}

bool FileUtils::createDirectory(const std::string& path) {
    return std::filesystem::create_directories(path);
}

bool FileUtils::remove(const std::string& path) {
    return std::filesystem::remove(path);
}

std::vector<std::string> FileUtils::listDirectory(const std::string& path) {
    std::vector<std::string> entries;

    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
        return entries;
    }

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        entries.push_back(entry.path().string());
    }

    return entries;
}

std::vector<std::string> FileUtils::findFiles(const std::string& directory, const std::string& pattern) {
    std::vector<std::string> matches;
    std::regex regex(pattern);

    if (!std::filesystem::exists(directory)) {
        return matches;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (std::regex_match(entry.path().filename().string(), regex)) {
            matches.push_back(entry.path().string());
        }
    }

    return matches;
}

std::string FileUtils::getExtension(const std::string& path) {
    return std::filesystem::path(path).extension().string();
}

std::string FileUtils::getFileName(const std::string& path) {
    return std::filesystem::path(path).filename().string();
}

std::string FileUtils::getDirectory(const std::string& path) {
    return std::filesystem::path(path).parent_path().string();
}

std::string FileUtils::join(const std::string& path1, const std::string& path2) {
    return (std::filesystem::path(path1) / path2).string();
}

} // namespace SoundBoost
