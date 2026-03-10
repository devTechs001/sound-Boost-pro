// src/network/LicenseValidator.hpp
#pragma once

#include <string>

namespace SoundBoost {

class LicenseValidator {
public:
    static LicenseValidator& getInstance();

    bool validate(const std::string& licenseKey);
    bool isLicensed() const { return m_licensed; }
    std::string getLicenseType() const { return m_licenseType; }

private:
    LicenseValidator() = default;
    bool m_licensed{false};
    std::string m_licenseType;
};

} // namespace SoundBoost
