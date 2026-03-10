# src/services/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

project(SoundBoostServices LANGUAGES CXX)

set(SERVICES_SOURCES
    ServiceLocator.cpp
    EventBus.cpp
    AudioService.cpp
    SettingsService.cpp
    ProfileService.cpp
)

qt_add_library(SoundBoostServices STATIC ${SERVICES_SOURCES})

target_include_directories(SoundBoostServices PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(SoundBoostServices PUBLIC
    SoundBoostCore
    SoundBoostUtils
    Qt6::Core
)
