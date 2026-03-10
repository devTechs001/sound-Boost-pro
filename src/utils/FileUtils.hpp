// src/utils/FileUtils.hpp
#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace SoundBoost {

class FileUtils {
public:
    static std::string readFile(const std::string& path);
    static void writeFile(const std::string& path, const std::string& content);
    static std::vector<std::string> readLines(const std::string& path);

    static bool exists(const std::string& path);
    static bool isDirectory(const std::string& path);
    static bool createDirectory(const std::string& path);
    static bool remove(const std::string& path);

    static std::vector<std::string> listDirectory(const std::string& path);
    static std::vector<std::string> findFiles(const std::string& directory, const std::string& pattern);

    static std::string getExtension(const std::string& path);
    static std::string getFileName(const std::string& path);
    static std::string getDirectory(const std::string& path);

    static std::string join(const std::string& path1, const std::string& path2);
};

} // namespace SoundBoost
