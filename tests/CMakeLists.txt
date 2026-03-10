# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

project(SoundBoostTests LANGUAGES CXX)

# Unit tests
set(UNIT_TESTS
    unit/test_audio_buffer.cpp
    unit/test_dsp_processor.cpp
    unit/test_effects.cpp
    unit/test_ml_engine.cpp
    unit/test_biquad_filter.cpp
    unit/test_fft.cpp
)

# Integration tests
set(INTEGRATION_TESTS
    integration/test_audio_pipeline.cpp
    integration/test_effect_chain.cpp
    integration/test_platform_audio.cpp
)

# Performance benchmarks
set(BENCHMARKS
    performance/benchmark_dsp.cpp
    performance/benchmark_ml.cpp
    performance/benchmark_simd.cpp
)

# Unit tests executable
add_executable(SoundBoostUnitTests ${UNIT_TESTS})
target_link_libraries(SoundBoostUnitTests PRIVATE
    SoundBoostCore
    SoundBoostUtils
    GTest::gtest_main
)

# Integration tests executable
add_executable(SoundBoostIntegrationTests ${INTEGRATION_TESTS})
target_link_libraries(SoundBoostIntegrationTests PRIVATE
    SoundBoostCore
    SoundBoostUtils
    SoundBoostPlatform
    GTest::gtest_main
)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(SoundBoostUnitTests)
gtest_discover_tests(SoundBoostIntegrationTests)

# Benchmarks (optional)
if(BUILD_BENCHMARKS)
    find_package(benchmark QUIET)
    if(benchmark_FOUND)
        add_executable(SoundBoostBenchmarks ${BENCHMARKS})
        target_link_libraries(SoundBoostBenchmarks PRIVATE
            SoundBoostCore
            benchmark::benchmark
        )
    endif()
endif()
