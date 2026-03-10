# src/gui/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

project(SoundBoostGUI LANGUAGES CXX)

set(GUI_SOURCES
    MainWindow.cpp
    themes/ThemeManager.cpp
    views/HomeView.cpp
)

set(GUI_HEADERS
    MainWindow.hpp
    themes/ThemeManager.hpp
    views/HomeView.hpp
    views/EqualizerView.hpp
    views/BassBoostView.hpp
    views/EffectsView.hpp
    views/AnalyzerView.hpp
    views/SettingsView.hpp
    views/MLInsightsView.hpp
    components/VUMeter.hpp
    components/SpectrumWidget.hpp
    components/EQBandWidget.hpp
    components/Knob.hpp
    components/Slider.hpp
    components/ToggleSwitch.hpp
    components/WaveformWidget.hpp
    components/PresetSelector.hpp
    components/DeviceSelector.hpp
    components/CircularProgress.hpp
)

qt_add_library(SoundBoostGUI STATIC
    ${GUI_SOURCES}
    ${GUI_HEADERS}
)

target_include_directories(SoundBoostGUI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(SoundBoostGUI PUBLIC
    SoundBoostCore
    SoundBoostServices
    SoundBoostUtils
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
)
