// src/Application.hpp
#pragma once

#include <memory>
#include <string>
#include <atomic>

namespace SoundBoost {

class AudioEngine;
class MainWindow;
class ServiceLocator;
class Configuration;

class Application {
public:
    Application();
    ~Application();

    // Non-copyable, non-movable
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    bool initialize();
    int run();
    void shutdown();

    void setStartMinimized(bool minimized) { m_startMinimized = minimized; }
    void setStartupProfile(const std::string& profile) { m_startupProfile = profile; }

    // Singleton access
    static Application& getInstance();

    // Component access
    AudioEngine& getAudioEngine();
    ServiceLocator& getServices();

private:
    bool initializeServices();
    bool initializeAudioEngine();
    bool initializeGUI();
    bool initializeML();
    bool loadPlugins();
    void setupConnections();

    std::unique_ptr<ServiceLocator> m_services;
    std::unique_ptr<AudioEngine> m_audioEngine;
    std::unique_ptr<MainWindow> m_mainWindow;

    bool m_startMinimized{false};
    std::string m_startupProfile;
    std::atomic<bool> m_running{false};

    static Application* s_instance;
};

} // namespace SoundBoost
