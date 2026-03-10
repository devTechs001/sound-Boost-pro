// src/main.cpp
#include "Application.hpp"
#include "utils/Logger.hpp"
#include "utils/Configuration.hpp"
#include "services/ServiceLocator.hpp"

#include <QApplication>
#include <QStyleFactory>
#include <QSurfaceFormat>
#include <QCommandLineParser>

#include <csignal>
#include <exception>

namespace {
    void setupSignalHandlers() {
        auto signalHandler = [](int signal) {
            LOG_INFO("Received signal {}, shutting down gracefully...", signal);
            QApplication::quit();
        };

        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
#ifdef PLATFORM_WINDOWS
        std::signal(SIGBREAK, signalHandler);
#endif
    }

    void setupOpenGL() {
        QSurfaceFormat format;
        format.setVersion(3, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setSamples(4);
        format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
        QSurfaceFormat::setDefaultFormat(format);
    }
}

int main(int argc, char* argv[]) {
    // High DPI support
    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    setupOpenGL();
    setupSignalHandlers();

    QApplication qtApp(argc, argv);
    QApplication::setApplicationName("SoundBoost Pro");
    QApplication::setApplicationVersion(SOUNDBOOST_VERSION);
    QApplication::setOrganizationName("SoundBoost Technologies");
    QApplication::setOrganizationDomain("soundboostpro.com");

    // Command line parsing
    QCommandLineParser parser;
    parser.setApplicationDescription("Enterprise Audio Enhancement Suite");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption debugOption("debug", "Enable debug logging");
    QCommandLineOption minimizedOption("minimized", "Start minimized to tray");
    QCommandLineOption configOption("config", "Configuration file path", "file");
    QCommandLineOption profileOption("profile", "Load specific audio profile", "name");

    parser.addOptions({debugOption, minimizedOption, configOption, profileOption});
    parser.process(qtApp);

    try {
        // Initialize logging
        SoundBoost::Logger::getInstance().initialize(
            parser.isSet(debugOption) ?
            SoundBoost::LogLevel::Debug : SoundBoost::LogLevel::Info
        );

        LOG_INFO("Starting SoundBoost Pro v{}", SOUNDBOOST_VERSION);

        // Load configuration
        std::string configPath = parser.isSet(configOption) ?
            parser.value(configOption).toStdString() : "";
        SoundBoost::Configuration::getInstance().load(configPath);

        // Create and run application
        SoundBoost::Application app;

        if (parser.isSet(profileOption)) {
            app.setStartupProfile(parser.value(profileOption).toStdString());
        }

        app.setStartMinimized(parser.isSet(minimizedOption));

        if (!app.initialize()) {
            LOG_ERROR("Failed to initialize application");
            return 1;
        }

        return app.run();

    } catch (const std::exception& e) {
        LOG_CRITICAL("Unhandled exception: {}", e.what());
        return 1;
    }
}
