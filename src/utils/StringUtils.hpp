// src/utils/StringUtils.hpp
#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

namespace SoundBoost {

class StringUtils {
public:
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string join(const std::vector<std::string>& parts, const std::string& delimiter);

    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);

    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);

    static std::string replace(const std::string& str, const std::string& from, const std::string& to);
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);

    static bool isNumeric(const std::string& str);
    static int toInt(const std::string& str, int defaultValue = 0);
    static float toFloat(const std::string& str, float defaultValue = 0.0f);
    static double toDouble(const std::string& str, double defaultValue = 0.0);

    static std::string format(const char* format, ...);
};

} // namespace SoundBoost
