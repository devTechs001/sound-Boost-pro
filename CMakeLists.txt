# CMakeLists.txt - Root
cmake_minimum_required(VERSION 3.20)

# Project configuration
project(SoundBoostPro
    VERSION 2.0.0
    DESCRIPTION "Enterprise Cross-Platform Audio Enhancement Suite"
    LANGUAGES CXX C
)

# C++20 Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_DOCS "Build documentation" ON)
option(BUILD_BENCHMARKS "Build performance benchmarks" ON)
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(ENABLE_TSAN "Enable Thread Sanitizer" OFF)
option(ENABLE_UBSAN "Enable Undefined Behavior Sanitizer" OFF)
option(ENABLE_LTO "Enable Link Time Optimization" ON)
option(ENABLE_ML "Enable Machine Learning features" ON)
option(ENABLE_SIMD "Enable SIMD optimizations" ON)
option(ENABLE_PROFILING "Enable profiling support" OFF)

# Platform detection
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(PLATFORM_WINDOWS TRUE)
    add_definitions(-DPLATFORM_WINDOWS)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(PLATFORM_LINUX TRUE)
    add_definitions(-DPLATFORM_LINUX)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(PLATFORM_MACOS TRUE)
    add_definitions(-DPLATFORM_MACOS)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(PLATFORM_ANDROID TRUE)
    add_definitions(-DPLATFORM_ANDROID)
elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(PLATFORM_IOS TRUE)
    add_definitions(-DPLATFORM_IOS)
endif()

# Compiler-specific settings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter
        -ffast-math
        $<$<BOOL:${ENABLE_SIMD}>:-march=native>
        $<$<BOOL:${ENABLE_ASAN}>:-fsanitize=address>
        $<$<BOOL:${ENABLE_TSAN}>:-fsanitize=thread>
        $<$<BOOL:${ENABLE_UBSAN}>:-fsanitize=undefined>
    )
    if(ENABLE_LTO)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
elseif(MSVC)
    add_compile_options(
        /W4 /WX-
        /fp:fast
        /arch:AVX2
        /MP
        /permissive-
    )
    if(ENABLE_LTO)
        add_compile_options(/GL)
        add_link_options(/LTCG)
    endif()
endif()

# Find dependencies
find_package(Qt6 REQUIRED COMPONENTS
    Core Widgets Gui Multimedia
    Concurrent Network LinguistTools
)
find_package(Threads REQUIRED)

if(ENABLE_ML)
    find_package(onnxruntime QUIET)
    if(NOT onnxruntime_FOUND)
        # Fallback to manual ONNX Runtime setup
        include(FetchContent)
        FetchContent_Declare(
            onnxruntime
            URL https://github.com/microsoft/onnxruntime/releases/download/v1.16.0/onnxruntime-linux-x64-1.16.0.tgz
        )
        FetchContent_MakeAvailable(onnxruntime)
    endif()
endif()

# Platform-specific dependencies
if(PLATFORM_WINDOWS)
    # Windows Audio Session API
    set(PLATFORM_LIBS ole32 oleaut32 uuid winmm ksuser)
elseif(PLATFORM_LINUX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(PULSEAUDIO REQUIRED libpulse)
    pkg_check_modules(PIPEWIRE pipewire-0.3)
    pkg_check_modules(ALSA REQUIRED alsa)
    set(PLATFORM_LIBS ${PULSEAUDIO_LIBRARIES} ${ALSA_LIBRARIES})
    if(PIPEWIRE_FOUND)
        list(APPEND PLATFORM_LIBS ${PIPEWIRE_LIBRARIES})
        add_definitions(-DHAVE_PIPEWIRE)
    endif()
elseif(PLATFORM_MACOS)
    find_library(COREAUDIO CoreAudio REQUIRED)
    find_library(AUDIOTOOLBOX AudioToolbox REQUIRED)
    find_library(COREFOUNDATION CoreFoundation REQUIRED)
    set(PLATFORM_LIBS ${COREAUDIO} ${AUDIOTOOLBOX} ${COREFOUNDATION})
elseif(PLATFORM_ANDROID)
    find_library(AAUDIO_LIB aaudio)
    find_library(OPENSLES_LIB OpenSLES)
    find_library(LOG_LIB log)
    set(PLATFORM_LIBS ${AAUDIO_LIB} ${OPENSLES_LIB} ${LOG_LIB})
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third_party
    ${Qt6Core_INCLUDE_DIRS}
)

# Third-party libraries
add_subdirectory(third_party)

# Source subdirectories
add_subdirectory(src/utils)
add_subdirectory(src/core)
add_subdirectory(src/platform)
add_subdirectory(src/gui)
add_subdirectory(src/services)
add_subdirectory(src/network)

# Main executable
set(MAIN_SOURCES
    src/main.cpp
    src/Application.cpp
)

if(PLATFORM_WINDOWS)
    list(APPEND MAIN_SOURCES resources/app.rc)
endif()

qt_add_executable(SoundBoostPro
    ${MAIN_SOURCES}
)

target_link_libraries(SoundBoostPro PRIVATE
    SoundBoostCore
    SoundBoostGUI
    SoundBoostPlatform
    SoundBoostServices
    SoundBoostNetwork
    SoundBoostUtils
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::Multimedia
    Qt6::Concurrent
    Qt6::Network
    Threads::Threads
    ${PLATFORM_LIBS}
)

# Set application properties
set_target_properties(SoundBoostPro PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.soundboostpro.app"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
)

# Testing
if(BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    add_subdirectory(tests)
endif()

# Documentation
if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        configure_file(
            ${CMAKE_SOURCE_DIR}/docs/api/doxygen/Doxyfile.in
            ${CMAKE_BINARY_DIR}/Doxyfile @ONLY
        )
        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating API documentation"
        )
    endif()
endif()

# Installation
include(GNUInstallDirs)
install(TARGETS SoundBoostPro
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# CPack configuration
set(CPACK_PACKAGE_NAME "SoundBoostPro")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "SoundBoost Technologies")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Enterprise Audio Enhancement Suite")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

if(PLATFORM_WINDOWS)
    set(CPACK_GENERATOR "NSIS;WIX;ZIP")
elseif(PLATFORM_LINUX)
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
elseif(PLATFORM_MACOS)
    set(CPACK_GENERATOR "DragNDrop;productbuild")
endif()

include(CPack)
