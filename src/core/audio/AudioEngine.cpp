// src/core/audio/AudioEngine.cpp
#include "AudioEngine.hpp"
#include "AudioCapture.hpp"
#include "AudioOutput.hpp"
#include "../ml/MLEngine.hpp"
#include "../../platform/PlatformAbstraction.hpp"
#include "../../utils/Logger.hpp"
#include "../../utils/Profiler.hpp"

#include <chrono>
#include <algorithm>

namespace SoundBoost {

AudioEngine::AudioEngine(QObject* parent)
    : QObject(parent)
    , m_effectChain(std::make_unique<EffectChain>())
    , m_dspProcessor(std::make_unique<DSPProcessor>())
    , m_spectrumAnalyzer(std::make_unique<SpectrumAnalyzer>())
    , m_mlEngine(std::make_unique<MLEngine>())
{
    LOG_DEBUG("AudioEngine created");
}

AudioEngine::~AudioEngine() {
    stop();
    LOG_DEBUG("AudioEngine destroyed");
}

bool AudioEngine::initialize(const AudioEngineConfig& config) {
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG_INFO("Initializing AudioEngine with {}Hz, {} samples buffer, {} channels",
        config.sampleRate, config.bufferSize, config.channels);

    m_config = config;

    // Initialize platform-specific audio capture
    m_capture = Platform::createAudioCapture();
    if (!m_capture->initialize(config.sampleRate, config.bufferSize, config.channels)) {
        LOG_ERROR("Failed to initialize audio capture");
        return false;
    }

    // Initialize audio output
    m_output = Platform::createAudioOutput();
    if (!m_output->initialize(config.sampleRate, config.bufferSize, config.channels)) {
        LOG_ERROR("Failed to initialize audio output");
        return false;
    }

    // Initialize DSP processor
    m_dspProcessor->initialize(config.sampleRate, config.bufferSize);

    // Initialize spectrum analyzer
    m_spectrumAnalyzer->initialize(config.sampleRate, 4096); // FFT size

    // Initialize effect chain with default effects
    m_effectChain->initialize(config.sampleRate, config.bufferSize);
    setupDefaultEffects();

    // Initialize ring buffers for lock-free audio processing
    size_t ringBufferSize = config.bufferSize * config.channels * 4;
    m_inputRingBuffer = std::make_unique<LockFreeRingBuffer<float>>(ringBufferSize);
    m_outputRingBuffer = std::make_unique<LockFreeRingBuffer<float>>(ringBufferSize);

    LOG_INFO("AudioEngine initialized successfully");
    return true;
}

void AudioEngine::setupDefaultEffects() {
    // Bass Booster
    auto bassBooster = std::make_unique<BassBooster>();
    bassBooster->setParameter("frequency", 100.0f);
    bassBooster->setParameter("gain", 6.0f);
    bassBooster->setParameter("width", 1.5f);
    m_effectChain->addEffect(std::move(bassBooster));

    // 10-band Equalizer
    auto equalizer = std::make_unique<Equalizer>(10);
    m_effectChain->addEffect(std::move(equalizer));

    // Compressor
    auto compressor = std::make_unique<Compressor>();
    compressor->setParameter("threshold", -12.0f);
    compressor->setParameter("ratio", 4.0f);
    compressor->setParameter("attack", 10.0f);
    compressor->setParameter("release", 100.0f);
    m_effectChain->addEffect(std::move(compressor));

    // Stereo Widener
    auto stereoWidener = std::make_unique<StereoWidener>();
    m_effectChain->addEffect(std::move(stereoWidener));

    // Limiter (always last)
    auto limiter = std::make_unique<Limiter>();
    limiter->setParameter("ceiling", -0.3f);
    m_effectChain->addEffect(std::move(limiter));
}

void AudioEngine::start() {
    if (m_running.exchange(true)) {
        return; // Already running
    }

    LOG_INFO("Starting AudioEngine...");

    // Start audio capture
    m_capture->setCallback([this](const AudioBuffer& buffer) {
        // Write to input ring buffer
        m_inputRingBuffer->write(buffer.getData(), buffer.getSize());
    });
    m_capture->start();

    // Start output
    m_output->start();

    // Start processing thread
    m_processingThread = std::make_unique<std::thread>([this]() {
        processAudio();
    });

    emit stateChanged(true);
    LOG_INFO("AudioEngine started");
}

void AudioEngine::stop() {
    if (!m_running.exchange(false)) {
        return; // Already stopped
    }

    LOG_INFO("Stopping AudioEngine...");

    // Stop capture and output
    if (m_capture) m_capture->stop();
    if (m_output) m_output->stop();

    // Wait for processing thread
    if (m_processingThread && m_processingThread->joinable()) {
        m_processingThread->join();
    }

    emit stateChanged(false);
    LOG_INFO("AudioEngine stopped");
}

void AudioEngine::processAudio() {
    const size_t bufferSize = m_config.bufferSize * m_config.channels;
    std::vector<float> inputBuffer(bufferSize);
    std::vector<float> outputBuffer(bufferSize);

    AudioBuffer audioBuffer(m_config.bufferSize, m_config.channels, m_config.sampleRate);

    while (m_running.load()) {
        PROFILE_SCOPE("AudioEngine::processAudio");

        // Read from input ring buffer
        size_t read = m_inputRingBuffer->read(inputBuffer.data(), bufferSize);

        if (read < bufferSize) {
            // Not enough data, sleep briefly
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        }

        // Copy to audio buffer
        audioBuffer.setData(inputBuffer.data(), bufferSize);

        if (!m_bypass.load() && m_masterEnabled.load()) {
            // Process through effect chain
            m_effectChain->process(audioBuffer);

            // Apply master volume
            float masterVol = m_masterVolume.load();
            audioBuffer.applyGain(masterVol);
        }

        // Update analysis
        updateAnalysis(audioBuffer);

        // Write to output
        m_output->write(audioBuffer);
    }
}

void AudioEngine::updateAnalysis(const AudioBuffer& buffer) {
    // Update spectrum analyzer
    m_spectrumAnalyzer->process(buffer);

    // Get spectrum data
    auto spectrum = m_spectrumAnalyzer->getMagnitudes();
    emit spectrumDataReady(spectrum);

    // Calculate levels
    auto [left, right] = buffer.getPeakLevels();
    emit audioLevelsChanged(left, right);
}

std::vector<AudioDevice> AudioEngine::getInputDevices() const {
    return m_capture ? m_capture->getAvailableDevices() : std::vector<AudioDevice>{};
}

std::vector<AudioDevice> AudioEngine::getOutputDevices() const {
    return m_output ? m_output->getAvailableDevices() : std::vector<AudioDevice>{};
}

bool AudioEngine::setInputDevice(const std::string& deviceId) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_capture) return false;

    bool wasRunning = m_running.load();
    if (wasRunning) stop();

    bool success = m_capture->setDevice(deviceId);

    if (wasRunning) start();

    if (success) {
        emit deviceChanged("input", QString::fromStdString(deviceId));
    }

    return success;
}

bool AudioEngine::setOutputDevice(const std::string& deviceId) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_output) return false;

    bool wasRunning = m_running.load();
    if (wasRunning) stop();

    bool success = m_output->setDevice(deviceId);

    if (wasRunning) start();

    if (success) {
        emit deviceChanged("output", QString::fromStdString(deviceId));
    }

    return success;
}

void AudioEngine::setMasterVolume(float volume) {
    m_masterVolume.store(std::clamp(volume, 0.0f, 2.0f));
}

void AudioEngine::setMasterEnabled(bool enabled) {
    m_masterEnabled.store(enabled);
}

void AudioEngine::setBypass(bool bypass) {
    m_bypass.store(bypass);
}

void AudioEngine::reset() {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_effectChain->reset();
    m_inputRingBuffer->clear();
    m_outputRingBuffer->clear();
}

} // namespace SoundBoost
