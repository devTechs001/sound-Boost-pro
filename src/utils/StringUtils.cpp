// src/utils/StringUtils.cpp
#include "StringUtils.hpp"
#include <cstdarg>
#include <cstdio>

namespace SoundBoost {

std::vector<std::string> StringUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;

    while (std::getline(iss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    return tokens;
}

std::string StringUtils::join(const std::vector<std::string>& parts, const std::string& delimiter) {
    std::string result;

    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) {
            result += delimiter;
        }
        result += parts[i];
    }

    return result;
}

std::string StringUtils::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";

    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::string StringUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string StringUtils::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

bool StringUtils::startsWith(const std::string& str, const std::string& prefix) {
    if (prefix.size() > str.size()) return false;
    return str.compare(0, prefix.size(), prefix) == 0;
}

bool StringUtils::endsWith(const std::string& str, const std::string& suffix) {
    if (suffix.size() > str.size()) return false;
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string StringUtils::replace(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = result.find(from);
    if (pos != std::string::npos) {
        result.replace(pos, from.size(), to);
    }
    return result;
}

std::string StringUtils::replaceAll(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = 0;

    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.size(), to);
        pos += to.size();
    }

    return result;
}

bool StringUtils::isNumeric(const std::string& str) {
    if (str.empty()) return false;

    auto it = str.begin();
    if (*it == '-' || *it == '+') ++it;

    bool hasDigit = false;
    bool hasDot = false;

    while (it != str.end()) {
        if (std::isdigit(*it)) {
            hasDigit = true;
        } else if (*it == '.') {
            if (hasDot) return false;
            hasDot = true;
        } else {
            return false;
        }
        ++it;
    }

    return hasDigit;
}

int StringUtils::toInt(const std::string& str, int defaultValue) {
    try {
        return std::stoi(str);
    } catch (...) {
        return defaultValue;
    }
}

float StringUtils::toFloat(const std::string& str, float defaultValue) {
    try {
        return std::stof(str);
    } catch (...) {
        return defaultValue;
    }
}

double StringUtils::toDouble(const std::string& str, double defaultValue) {
    try {
        return std::stod(str);
    } catch (...) {
        return defaultValue;
    }
}

std::string StringUtils::format(const char* format, ...) {
    va_list args;
    va_start(args, format);

    va_list argsCopy;
    va_copy(argsCopy, args);

    int size = std::vsnprintf(nullptr, 0, format, argsCopy);
    va_end(argsCopy);

    if (size < 0) {
        va_end(args);
        return "";
    }

    std::vector<char> buffer(size + 1);
    std::vsnprintf(buffer.data(), buffer.size(), format, args);
    va_end(args);

    return std::string(buffer.data());
}

} // namespace SoundBoost
