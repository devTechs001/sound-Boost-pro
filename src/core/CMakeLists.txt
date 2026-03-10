# src/core/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

project(SoundBoostCore LANGUAGES CXX)

# Audio module
set(AUDIO_SOURCES
    audio/AudioEngine.cpp
    audio/AudioBuffer.cpp
)

# DSP module
set(DSP_SOURCES
    dsp/DSPProcessor.cpp
    dsp/BiquadFilter.cpp
    dsp/FilterDesign.cpp
    dsp/FFTProcessor.cpp
    dsp/SIMD/SIMDDispatcher.cpp
)

# Effects module
set(EFFECTS_SOURCES
    effects/EffectBase.cpp
    effects/EffectChain.cpp
    effects/BassBooster.cpp
    effects/Equalizer.cpp
    effects/Compressor.cpp
    effects/Limiter.cpp
    effects/StereoWidener.cpp
)

# ML module
set(ML_SOURCES
    ml/MLEngine.cpp
    ml/FeatureExtractor.cpp
    ml/BeatDetector.cpp
    ml/AdaptiveEQ.cpp
)

# Analysis module
set(ANALYSIS_SOURCES
    analysis/SpectrumAnalyzer.cpp
    analysis/LevelMeter.cpp
    analysis/WaveformAnalyzer.cpp
)

# Create core library
add_library(SoundBoostCore STATIC
    ${AUDIO_SOURCES}
    ${DSP_SOURCES}
    ${EFFECTS_SOURCES}
    ${ML_SOURCES}
    ${ANALYSIS_SOURCES}
)

target_include_directories(SoundBoostCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(SoundBoostCore PUBLIC
    Qt6::Core
    Threads::Threads
)

if(ENABLE_ML)
    target_link_libraries(SoundBoostCore PRIVATE onnxruntime)
endif()
