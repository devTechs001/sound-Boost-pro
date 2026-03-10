// src/platform/PlatformAbstraction.cpp
#include "PlatformAbstraction.hpp"
#include "../utils/Logger.hpp"

#ifdef PLATFORM_WINDOWS
    #include "windows/WindowsAudioCapture.hpp"
#elif defined(PLATFORM_LINUX)
    #include "linux/LinuxAudioCapture.hpp"
#elif defined(PLATFORM_MACOS)
    #include "macos/MacOSAudioCapture.hpp"
#endif

namespace SoundBoost {
namespace Platform {

std::unique_ptr<AudioCapture> createAudioCapture() {
#ifdef PLATFORM_WINDOWS
    return std::make_unique<WindowsAudioCapture>();
#elif defined(PLATFORM_LINUX)
    return std::make_unique<LinuxAudioCapture>();
#elif defined(PLATFORM_MACOS)
    return std::make_unique<MacOSAudioCapture>();
#else
    LOG_ERROR("Unsupported platform for audio capture");
    return nullptr;
#endif
}

std::unique_ptr<AudioOutput> createAudioOutput() {
#ifdef PLATFORM_WINDOWS
    return std::make_unique<WindowsAudioOutput>();
#elif defined(PLATFORM_LINUX)
    return std::make_unique<LinuxAudioOutput>();
#elif defined(PLATFORM_MACOS)
    return std::make_unique<MacOSAudioOutput>();
#else
    LOG_ERROR("Unsupported platform for audio output");
    return nullptr;
#endif
}

std::string getPlatformName() {
#ifdef PLATFORM_WINDOWS
    return "Windows";
#elif defined(PLATFORM_LINUX)
    return "Linux";
#elif defined(PLATFORM_MACOS)
    return "macOS";
#elif defined(PLATFORM_ANDROID)
    return "Android";
#elif defined(PLATFORM_IOS)
    return "iOS";
#else
    return "Unknown";
#endif
}

std::string getOSVersion() {
    // Platform-specific version detection
#ifdef PLATFORM_WINDOWS
    return "Windows";
#elif defined(PLATFORM_LINUX)
    return "Linux";
#elif defined(PLATFORM_MACOS)
    return "macOS";
#else
    return "Unknown";
#endif
}

} // namespace Platform
} // namespace SoundBoost
