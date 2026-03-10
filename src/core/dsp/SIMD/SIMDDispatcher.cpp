// src/core/dsp/SIMD/SIMDDispatcher.cpp
#include "SIMDDispatcher.hpp"

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    #include <immintrin.h>
    #define HAS_X86_SIMD 1
#elif defined(__ARM_NEON) || defined(__ARM_NEON__)
    #include <arm_neon.h>
    #define HAS_NEON 1
#endif

#include <algorithm>
#include <cstring>
#include <cmath>

namespace SoundBoost {
namespace SIMD {

#ifdef HAS_X86_SIMD
static void cpuid(int info[4], int infoType) {
#ifdef _MSC_VER
    __cpuidex(info, infoType, 0);
#else
    __asm__ __volatile__ (
        "cpuid"
        : "=a"(info[0]), "=b"(info[1]), "=c"(info[2]), "=d"(info[3])
        : "a"(infoType), "c"(0)
    );
#endif
}

bool hasSSE() {
    int info[4];
    cpuid(info, 1);
    return (info[3] & (1 << 25)) != 0;
}

bool hasSSE2() {
    int info[4];
    cpuid(info, 1);
    return (info[3] & (1 << 26)) != 0;
}

bool hasSSE4() {
    int info[4];
    cpuid(info, 1);
    return (info[2] & (1 << 19)) != 0;
}

bool hasAVX() {
    int info[4];
    cpuid(info, 1);
    return (info[2] & (1 << 28)) != 0;
}

bool hasAVX2() {
    int info[4];
    cpuid(info, 7);
    return (info[1] & (1 << 5)) != 0;
}

bool hasAVX512() {
    int info[4];
    cpuid(info, 7);
    return (info[1] & (1 << 16)) != 0;
}
#endif

#ifdef HAS_NEON
bool hasNEON() { return true; }
#else
bool hasNEON() { return false; }
#endif

SIMDLevel detectSIMDLevel() {
#ifdef HAS_X86_SIMD
    if (hasAVX512()) return SIMDLevel::AVX512;
    if (hasAVX2()) return SIMDLevel::AVX2;
    if (hasAVX()) return SIMDLevel::AVX;
    if (hasSSE4()) return SIMDLevel::SSE4;
    if (hasSSE2()) return SIMDLevel::SSE2;
    if (hasSSE()) return SIMDLevel::SSE;
#endif
#ifdef HAS_NEON
    return SIMDLevel::NEON;
#endif
    return SIMDLevel::None;
}

// Scalar fallbacks
static void addScalar(float* dst, const float* src, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        dst[i] += src[i];
    }
}

static void multiplyScalar(float* data, float scalar, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        data[i] *= scalar;
    }
}

static void clipScalar(float* data, float minVal, float maxVal, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        data[i] = std::clamp(data[i], minVal, maxVal);
    }
}

static float sumScalar(const float* data, size_t count) {
    float result = 0.0f;
    for (size_t i = 0; i < count; ++i) {
        result += data[i];
    }
    return result;
}

// Public API - dispatches to best implementation
void add(float* dst, const float* src, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        dst[i] += src[i];
    }
}

void addWithGain(float* dst, const float* src, float gain, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        dst[i] += src[i] * gain;
    }
}

void multiply(float* data, float scalar, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        data[i] *= scalar;
    }
}

void clip(float* data, float min, float max, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        data[i] = std::clamp(data[i], min, max);
    }
}

float sum(const float* data, size_t count) {
    return sumScalar(data, count);
}

void copy(float* dst, const float* src, size_t count) {
    std::memcpy(dst, src, count * sizeof(float));
}

void clear(float* data, size_t count) {
    std::memset(data, 0, count * sizeof(float));
}

float findMax(const float* data, size_t count) {
    if (count == 0) return 0.0f;
    return *std::max_element(data, data + count);
}

float findMin(const float* data, size_t count) {
    if (count == 0) return 0.0f;
    return *std::min_element(data, data + count);
}

void multiplyAdd(float* dst, const float* src1, const float* src2, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        dst[i] += src1[i] * src2[i];
    }
}

void abs(float* dst, const float* src, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        dst[i] = std::abs(src[i]);
    }
}

float dotProduct(const float* a, const float* b, size_t count) {
    float result = 0.0f;
    for (size_t i = 0; i < count; ++i) {
        result += a[i] * b[i];
    }
    return result;
}

void complexMultiply(float* realDst, float* imagDst,
                     const float* realA, const float* imagA,
                     const float* realB, const float* imagB,
                     size_t count) {
    for (size_t i = 0; i < count; ++i) {
        realDst[i] = realA[i] * realB[i] - imagA[i] * imagB[i];
        imagDst[i] = realA[i] * imagB[i] + imagA[i] * realB[i];
    }
}

void interleave(float* dst, const float* const* src, int channels, size_t samples) {
    for (size_t s = 0; s < samples; ++s) {
        for (int c = 0; c < channels; ++c) {
            dst[s * channels + c] = src[c][s];
        }
    }
}

void deinterleave(float* const* dst, const float* src, int channels, size_t samples) {
    for (size_t s = 0; s < samples; ++s) {
        for (int c = 0; c < channels; ++c) {
            dst[c][s] = src[s * channels + c];
        }
    }
}

void stereoToMono(float* dst, const float* src, size_t samples) {
    for (size_t i = 0; i < samples; ++i) {
        dst[i] = (src[i * 2] + src[i * 2 + 1]) * 0.5f;
    }
}

void monoToStereo(float* dst, const float* src, size_t samples) {
    for (size_t i = 0; i < samples; ++i) {
        dst[i * 2] = src[i];
        dst[i * 2 + 1] = src[i];
    }
}

void applyWindow(float* data, const float* window, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        data[i] *= window[i];
    }
}

} // namespace SIMD
} // namespace SoundBoost
