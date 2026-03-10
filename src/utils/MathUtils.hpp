// src/utils/MathUtils.hpp
#pragma once

#include <cmath>
#include <algorithm>
#include <numbers>

namespace SoundBoost {

constexpr float PI = std::numbers::pi_v<float>;
constexpr float TWO_PI = 2.0f * PI;
constexpr float HALF_PI = PI / 2.0f;

inline float dbToLinear(float db) {
    return std::pow(10.0f, db / 20.0f);
}

inline float linearToDb(float linear) {
    return 20.0f * std::log10(std::max(linear, 1e-6f));
}

inline float hzToRad(float freq, float sampleRate) {
    return TWO_PI * freq / sampleRate;
}

inline float radToHz(float rad, float sampleRate) {
    return rad * sampleRate / TWO_PI;
}

inline float melToHz(float mel) {
    return 700.0f * (std::pow(10.0f, mel / 2595.0f) - 1.0f);
}

inline float hzToMel(float hz) {
    return 2595.0f * std::log10(1.0f + hz / 700.0f);
}

inline float midiToHz(float midi) {
    return 440.0f * std::pow(2.0f, (midi - 69.0f) / 12.0f);
}

inline float hzToMidi(float hz) {
    return 69.0f + 12.0f * std::log2(hz / 440.0f);
}

inline float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

inline float clamp(float value, float min, float max) {
    return std::max(min, std::min(value, max));
}

inline float smoothstep(float edge0, float edge1, float x) {
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

inline float smoothclamp(float value, float min, float max) {
    return smoothstep(min, max, value);
}

// Window functions
inline float hannWindow(float n, float N) {
    return 0.5f * (1.0f - std::cos(TWO_PI * n / N));
}

inline float hammingWindow(float n, float N) {
    return 0.54f - 0.46f * std::cos(TWO_PI * n / N);
}

inline float blackmanWindow(float n, float N) {
    return 0.42f - 0.5f * std::cos(TWO_PI * n / N) + 0.08f * std::cos(4.0f * PI * n / N);
}

inline float gaussianWindow(float n, float N, float sigma = 0.4f) {
    float x = (n - N / 2.0f) / (N / 2.0f);
    return std::exp(-0.5f * (x / sigma) * (x / sigma));
}

} // namespace SoundBoost
