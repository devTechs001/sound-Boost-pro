// src/platform/PlatformAbstraction.hpp
#pragma once

#include "../core/audio/AudioCapture.hpp"
#include "../core/audio/AudioOutput.hpp"

namespace SoundBoost {
namespace Platform {

std::unique_ptr<AudioCapture> createAudioCapture();
std::unique_ptr<AudioOutput> createAudioOutput();

std::string getPlatformName();
std::string getOSVersion();

} // namespace Platform
} // namespace SoundBoost
