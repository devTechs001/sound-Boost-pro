// src/network/LicenseValidator.cpp
#include "LicenseValidator.hpp"

namespace SoundBoost {

LicenseValidator& LicenseValidator::getInstance() {
    static LicenseValidator instance;
    return instance;
}

bool LicenseValidator::validate(const std::string& licenseKey) {
    // License validation logic
    if (!licenseKey.empty()) {
        m_licensed = true;
        m_licenseType = "Professional";
        return true;
    }
    return false;
}

} // namespace SoundBoost
