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

// AVX2 implementations
#ifdef HAS_X86_SIMD
static void addAVX2(float* dst, const float* src, size_t count) {
    size_t i = 0;
    for (; i + 8 <= count; i += 8) {
        __m256 a = _mm256_loadu_ps(dst + i);
        __m256 b = _mm256_loadu_ps(src + i);
        _mm256_storeu_ps(dst + i, _mm256_add_ps(a, b));
    }
    for (; i < count; ++i) {
        dst[i] += src[i];
    }
}

static void multiplyAVX2(float* data, float scalar, size_t count) {
    __m256 s = _mm256_set1_ps(scalar);
    size_t i = 0;
    for (; i + 8 <= count; i += 8) {
        __m256 v = _mm256_loadu_ps(data + i);
        _mm256_storeu_ps(data + i, _mm256_mul_ps(v, s));
    }
    for (; i < count; ++i) {
        data[i] *= scalar;
    }
}

static void clipAVX2(float* data, float minVal, float maxVal, size_t count) {
    __m256 minVec = _mm256_set1_ps(minVal);
    __m256 maxVec = _mm256_set1_ps(maxVal);
    size_t i = 0;
    for (; i + 8 <= count; i += 8) {
        __m256 v = _mm256_loadu_ps(data + i);
        v = _mm256_max_ps(v, minVec);
        v = _mm256_min_ps(v, maxVec);
        _mm256_storeu_ps(data + i, v);
    }
    for (; i < count; ++i) {
        data[i] = std::clamp(data[i], minVal, maxVal);
    }
}

static float sumAVX2(const float* data, size_t count) {
    __m256 sum = _mm256_setzero_ps();
    size_t i = 0;
    for (; i + 8 <= count; i += 8) {
        __m256 v = _mm256_loadu_ps(data + i);
        sum = _mm256_add_ps(sum, v);
    }

    // Horizontal sum
    __m128 hi = _mm256_extractf128_ps(sum, 1);
    __m128 lo = _mm256_castps256_ps128(sum);
    __m128 sum128 = _mm_add_ps(hi, lo);
    sum128 = _mm_hadd_ps(sum128, sum128);
    sum128 = _mm_hadd_ps(sum128, sum128);

    float result = _mm_cvtss_f32(sum128);
    for (; i < count; ++i) {
        result += data[i];
    }
    return result;
}
#endif

// NEON implementations
#ifdef HAS_NEON
static void addNEON(float* dst, const float* src, size_t count) {
    size_t i = 0;
    for (; i + 4 <= count; i += 4) {
        float32x4_t a = vld1q_f32(dst + i);
        float32x4_t b = vld1q_f32(src + i);
        vst1q_f32(dst + i, vaddq_f32(a, b));
    }
    for (; i < count; ++i) {
        dst[i] += src[i];
    }
}

static void multiplyNEON(float* data, float scalar, size_t count) {
    float32x4_t s = vdupq_n_f32(scalar);
    size_t i = 0;
    for (; i + 4 <= count; i += 4) {
        float32x4_t v = vld1q_f32(data + i);
        vst1q_f32(data + i, vmulq_f32(v, s));
    }
    for (; i < count; ++i) {
        data[i] *= scalar;
    }
}

static void clipNEON(float* data, float minVal, float maxVal, size_t count) {
    float32x4_t minVec = vdupq_n_f32(minVal);
    float32x4_t maxVec = vdupq_n_f32(maxVal);
    size_t i = 0;
    for (; i + 4 <= count; i += 4) {
        float32x4_t v = vld1q_f32(data + i);
        v = vmaxq_f32(v, minVec);
        v = vminq_f32(v, maxVec);
        vst1q_f32(data + i, v);
    }
    for (; i < count; ++i) {
        data[i] = std::clamp(data[i], minVal, maxVal);
    }
}
#endif

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
    static auto level = detectSIMDLevel();

#ifdef HAS_X86_SIMD
    if (level >= SIMDLevel::AVX2) {
        addAVX2(dst, src, count);
        return;
    }
#endif
#ifdef HAS_NEON
    if (level == SIMDLevel::NEON) {
        addNEON(dst, src, count);
        return;
    }
#endif
    addScalar(dst, src, count);
}

void addWithGain(float* dst, const float* src, float gain, size_t count) {
    static auto level = detectSIMDLevel();

#ifdef HAS_X86_SIMD
    if (level >= SIMDLevel::AVX2) {
        __m256 g = _mm256_set1_ps(gain);
        size_t i = 0;
        for (; i + 8 <= count; i += 8) {
            __m256 d = _mm256_loadu_ps(dst + i);
            __m256 s = _mm256_loadu_ps(src + i);
            _mm256_storeu_ps(dst + i, _mm256_fmadd_ps(s, g, d));
        }
        for (; i < count; ++i) {
            dst[i] += src[i] * gain;
        }
        return;
    }
#endif
    for (size_t i = 0; i < count; ++i) {
        dst[i] += src[i] * gain;
    }
}

void multiply(float* data, float scalar, size_t count) {
    static auto level = detectSIMDLevel();

#ifdef HAS_X86_SIMD
    if (level >= SIMDLevel::AVX2) {
        multiplyAVX2(data, scalar, count);
        return;
    }
#endif
#ifdef HAS_NEON
    if (level == SIMDLevel::NEON) {
        multiplyNEON(data, scalar, count);
        return;
    }
#endif
    multiplyScalar(data, scalar, count);
}

void clip(float* data, float min, float max, size_t count) {
    static auto level = detectSIMDLevel();

#ifdef HAS_X86_SIMD
    if (level >= SIMDLevel::AVX2) {
        clipAVX2(data, min, max, count);
        return;
    }
#endif
#ifdef HAS_NEON
    if (level == SIMDLevel::NEON) {
        clipNEON(data, min, max, count);
        return;
    }
#endif
    clipScalar(data, min, max, count);
}

float sum(const float* data, size_t count) {
    static auto level = detectSIMDLevel();

#ifdef HAS_X86_SIMD
    if (level >= SIMDLevel::AVX2) {
        return sumAVX2(data, count);
    }
#endif
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

    static auto level = detectSIMDLevel();

#ifdef HAS_X86_SIMD
    if (level >= SIMDLevel::AVX2 && count >= 8) {
        __m256 maxVec = _mm256_loadu_ps(data);
        size_t i = 8;
        for (; i + 8 <= count; i += 8) {
            __m256 v = _mm256_loadu_ps(data + i);
            maxVec = _mm256_max_ps(maxVec, v);
        }

        // Reduce
        __m128 hi = _mm256_extractf128_ps(maxVec, 1);
        __m128 lo = _mm256_castps256_ps128(maxVec);
        __m128 max128 = _mm_max_ps(hi, lo);
        max128 = _mm_max_ps(max128, _mm_shuffle_ps(max128, max128, _MM_SHUFFLE(1, 0, 3, 2)));
        max128 = _mm_max_ps(max128, _mm_shuffle_ps(max128, max128, _MM_SHUFFLE(2, 3, 0, 1)));

        float result = _mm_cvtss_f32(max128);
        for (; i < count; ++i) {
            result = std::max(result, data[i]);
        }
        return result;
    }
#endif

    return *std::max_element(data, data + count);
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
    static auto level = detectSIMDLevel();

#ifdef HAS_X86_SIMD
    if (level >= SIMDLevel::AVX2) {
        size_t i = 0;
        for (; i + 8 <= count; i += 8) {
            __m256 d = _mm256_loadu_ps(data + i);
            __m256 w = _mm256_loadu_ps(window + i);
            _mm256_storeu_ps(data + i, _mm256_mul_ps(d, w));
        }
        for (; i < count; ++i) {
            data[i] *= window[i];
        }
        return;
    }
#endif
    for (size_t i = 0; i < count; ++i) {
        data[i] *= window[i];
    }
}

} // namespace SIMD
} // namespace SoundBoost
