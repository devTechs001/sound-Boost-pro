# src/network/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

project(SoundBoostNetwork LANGUAGES CXX)

set(NETWORK_SOURCES
    LicenseValidator.cpp
    UpdateChecker.cpp
    TelemetryClient.cpp
    CloudSync.cpp
)

qt_add_library(SoundBoostNetwork STATIC ${NETWORK_SOURCES})

target_include_directories(SoundBoostNetwork PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(SoundBoostNetwork PUBLIC
    SoundBoostUtils
    Qt6::Core
    Qt6::Network
)
