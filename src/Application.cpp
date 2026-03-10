// src/Application.cpp
#include "Application.hpp"
#include "core/audio/AudioEngine.hpp"
#include "core/ml/MLEngine.hpp"
#include "gui/MainWindow.hpp"
#include "services/ServiceLocator.hpp"
#include "services/AudioService.hpp"
#include "services/SettingsService.hpp"
#include "services/ProfileService.hpp"
#include "utils/Logger.hpp"
#include "utils/Configuration.hpp"

#include <QApplication>

namespace SoundBoost {

Application* Application::s_instance = nullptr;

Application::Application() {
    s_instance = this;
    m_services = std::make_unique<ServiceLocator>();
}

Application::~Application() {
    shutdown();
    s_instance = nullptr;
}

Application& Application::getInstance() {
    assert(s_instance != nullptr);
    return *s_instance;
}

bool Application::initialize() {
    LOG_INFO("Initializing SoundBoost Pro...");

    if (!initializeServices()) {
        LOG_ERROR("Failed to initialize services");
        return false;
    }

    if (!initializeAudioEngine()) {
        LOG_ERROR("Failed to initialize audio engine");
        return false;
    }

    if (!initializeML()) {
        LOG_WARN("ML initialization failed, continuing without ML features");
    }

    if (!initializeGUI()) {
        LOG_ERROR("Failed to initialize GUI");
        return false;
    }

    if (!loadPlugins()) {
        LOG_WARN("Some plugins failed to load");
    }

    setupConnections();

    // Load startup profile if specified
    if (!m_startupProfile.empty()) {
        auto& profileService = m_services->get<ProfileService>();
        profileService.loadProfile(m_startupProfile);
    }

    m_running = true;
    LOG_INFO("SoundBoost Pro initialized successfully");
    return true;
}

bool Application::initializeServices() {
    LOG_DEBUG("Initializing services...");

    // Register core services
    m_services->registerService<SettingsService>(
        std::make_shared<SettingsService>());
    m_services->registerService<ProfileService>(
        std::make_shared<ProfileService>());
    m_services->registerService<AudioService>(
        std::make_shared<AudioService>());

    // Initialize all services
    for (auto& service : m_services->getAllServices()) {
        if (!service->initialize()) {
            LOG_ERROR("Failed to initialize service: {}", service->getName());
            return false;
        }
    }

    return true;
}

bool Application::initializeAudioEngine() {
    LOG_DEBUG("Initializing audio engine...");

    m_audioEngine = std::make_unique<AudioEngine>();

    auto& settings = m_services->get<SettingsService>();

    AudioEngineConfig config;
    config.sampleRate = settings.get<int>("audio.sample_rate", 48000);
    config.bufferSize = settings.get<int>("audio.buffer_size", 512);
    config.channels = settings.get<int>("audio.channels", 2);
    config.bitDepth = settings.get<int>("audio.bit_depth", 32);

    return m_audioEngine->initialize(config);
}

bool Application::initializeML() {
#ifdef ENABLE_ML
    LOG_DEBUG("Initializing ML engine...");

    auto& mlEngine = m_audioEngine->getMLEngine();

    auto& settings = m_services->get<SettingsService>();
    std::string modelPath = settings.get<std::string>(
        "ml.model_path", "models/");

    return mlEngine.loadModels(modelPath);
#else
    return true;
#endif
}

bool Application::initializeGUI() {
    LOG_DEBUG("Initializing GUI...");

    m_mainWindow = std::make_unique<MainWindow>(*m_audioEngine, *m_services);

    if (!m_mainWindow->initialize()) {
        return false;
    }

    if (!m_startMinimized) {
        m_mainWindow->show();
    } else {
        m_mainWindow->minimizeToTray();
    }

    return true;
}

bool Application::loadPlugins() {
    LOG_DEBUG("Loading plugins...");
    // Plugin loading logic here
    return true;
}

void Application::setupConnections() {
    LOG_DEBUG("Setting up signal connections...");

    // Connect audio engine signals to GUI
    QObject::connect(m_audioEngine.get(), &AudioEngine::audioLevelsChanged,
        m_mainWindow.get(), &MainWindow::updateAudioLevels);

    QObject::connect(m_audioEngine.get(), &AudioEngine::spectrumDataReady,
        m_mainWindow.get(), &MainWindow::updateSpectrum);

    // Connect settings changes
    auto& settings = m_services->get<SettingsService>();
    QObject::connect(&settings, &SettingsService::settingChanged,
        [this](const QString& key, const QVariant& value) {
            handleSettingChange(key.toStdString(), value);
        });
}

int Application::run() {
    LOG_INFO("Starting application main loop");

    // Start audio processing
    m_audioEngine->start();

    // Run Qt event loop
    int result = QApplication::exec();

    // Stop audio processing
    m_audioEngine->stop();

    return result;
}

void Application::shutdown() {
    if (!m_running.exchange(false)) {
        return;
    }

    LOG_INFO("Shutting down SoundBoost Pro...");

    // Save current settings
    auto& settings = m_services->get<SettingsService>();
    settings.save();

    // Cleanup in reverse order of initialization
    m_mainWindow.reset();
    m_audioEngine.reset();
    m_services.reset();

    LOG_INFO("SoundBoost Pro shutdown complete");
}

AudioEngine& Application::getAudioEngine() {
    return *m_audioEngine;
}

ServiceLocator& Application::getServices() {
    return *m_services;
}

} // namespace SoundBoost
