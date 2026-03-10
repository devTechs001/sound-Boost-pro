# src/platform/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

project(SoundBoostPlatform LANGUAGES CXX)

set(PLATFORM_SOURCES
    PlatformAbstraction.cpp
)

if(PLATFORM_LINUX)
    list(APPEND PLATFORM_SOURCES
        linux/LinuxAudioCapture.cpp
    )
endif()

if(PLATFORM_WINDOWS)
    list(APPEND PLATFORM_SOURCES
        windows/WindowsAudioCapture.cpp
    )
endif()

if(PLATFORM_MACOS)
    list(APPEND PLATFORM_SOURCES
        macos/MacOSAudioCapture.mm
    )
endif()

add_library(SoundBoostPlatform STATIC ${PLATFORM_SOURCES})

target_include_directories(SoundBoostPlatform PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(SoundBoostPlatform PUBLIC
    SoundBoostCore
    SoundBoostUtils
    Qt6::Core
    ${PLATFORM_LIBS}
)
