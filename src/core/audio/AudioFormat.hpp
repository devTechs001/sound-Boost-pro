// src/core/audio/AudioFormat.hpp
#pragma once

#include <cstdint>

namespace SoundBoost {

enum class SampleFormat {
    Float32,
    Float64,
    Int32,
    Int24,
    Int16,
    Int8
};

struct AudioFormat {
    SampleFormat sampleFormat{SampleFormat::Float32};
    int sampleRate{48000};
    int channels{2};
    int bitDepth{32};

    size_t bytesPerSample() const {
        switch (sampleFormat) {
            case SampleFormat::Float64: return 8;
            case SampleFormat::Int32: return 4;
            case SampleFormat::Float32: return 4;
            case SampleFormat::Int24: return 3;
            case SampleFormat::Int16: return 2;
            case SampleFormat::Int8: return 1;
            default: return 4;
        }
    }

    size_t bytesPerFrame() const {
        return bytesPerSample() * channels;
    }
};

} // namespace SoundBoost
