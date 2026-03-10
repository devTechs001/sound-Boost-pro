// src/core/dsp/SIMD/SIMDDispatcher.hpp
#pragma once

#include <cstddef>

namespace SoundBoost {
namespace SIMD {

// Feature detection
bool hasSSE();
bool hasSSE2();
bool hasSSE3();
bool hasSSE4();
bool hasAVX();
bool hasAVX2();
bool hasAVX512();
bool hasNEON();

// Optimal implementation selection
enum class SIMDLevel {
    None,
    SSE,
    SSE2,
    SSE4,
    AVX,
    AVX2,
    AVX512,
    NEON
};

SIMDLevel detectSIMDLevel();

// SIMD operations
void add(float* dst, const float* src, size_t count);
void addWithGain(float* dst, const float* src, float gain, size_t count);
void multiply(float* data, float scalar, size_t count);
void multiplyAdd(float* dst, const float* src1, const float* src2, size_t count);
void clip(float* data, float min, float max, size_t count);
void abs(float* dst, const float* src, size_t count);
void copy(float* dst, const float* src, size_t count);
void clear(float* data, size_t count);

float sum(const float* data, size_t count);
float dotProduct(const float* a, const float* b, size_t count);
float findMax(const float* data, size_t count);
float findMin(const float* data, size_t count);

// Complex operations
void complexMultiply(float* realDst, float* imagDst,
                     const float* realA, const float* imagA,
                     const float* realB, const float* imagB,
                     size_t count);

// Audio-specific operations
void interleave(float* dst, const float* const* src, int channels, size_t samples);
void deinterleave(float* const* dst, const float* src, int channels, size_t samples);
void stereoToMono(float* dst, const float* src, size_t samples);
void monoToStereo(float* dst, const float* src, size_t samples);
void applyWindow(float* data, const float* window, size_t count);

} // namespace SIMD
} // namespace SoundBoost
