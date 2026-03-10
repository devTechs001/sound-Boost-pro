# src/utils/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

project(SoundBoostUtils LANGUAGES CXX)

set(UTILS_SOURCES
    Logger.cpp
    Configuration.cpp
    ThreadPool.cpp
    Timer.cpp
    FileUtils.cpp
    StringUtils.cpp
)

add_library(SoundBoostUtils STATIC ${UTILS_SOURCES})

target_include_directories(SoundBoostUtils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(SoundBoostUtils PUBLIC
    Qt6::Core
    Threads::Threads
)
