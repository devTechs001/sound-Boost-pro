// src/core/audio/AudioEngine.hpp
#pragma once

#include "AudioDevice.hpp"
#include "AudioBuffer.hpp"
#include "AudioFormat.hpp"
#include "../effects/EffectChain.hpp"
#include "../dsp/DSPProcessor.hpp"
#include "../analysis/SpectrumAnalyzer.hpp"

#include <QObject>
#include <memory>
#include <vector>
#include <atomic>
#include <mutex>
#include <thread>
#include <functional>

namespace SoundBoost {

struct AudioEngineConfig {
    int sampleRate = 48000;
    int bufferSize = 512;
    int channels = 2;
    int bitDepth = 32;
    bool enableSystemCapture = true;
    bool enableLowLatency = true;
};

class MLEngine;
class AudioCapture;
class AudioOutput;

class AudioEngine : public QObject {
    Q_OBJECT

public:
    explicit AudioEngine(QObject* parent = nullptr);
    ~AudioEngine() override;

    // Lifecycle
    bool initialize(const AudioEngineConfig& config);
    void start();
    void stop();
    void reset();

    // Device management
    std::vector<AudioDevice> getInputDevices() const;
    std::vector<AudioDevice> getOutputDevices() const;
    bool setInputDevice(const std::string& deviceId);
    bool setOutputDevice(const std::string& deviceId);

    // Effect chain access
    EffectChain& getEffectChain() { return *m_effectChain; }
    const EffectChain& getEffectChain() const { return *m_effectChain; }

    // ML Engine access
    MLEngine& getMLEngine() { return *m_mlEngine; }

    // DSP access
    DSPProcessor& getDSPProcessor() { return *m_dspProcessor; }

    // Analysis
    SpectrumAnalyzer& getSpectrumAnalyzer() { return *m_spectrumAnalyzer; }

    // Master controls
    void setMasterVolume(float volume);
    float getMasterVolume() const { return m_masterVolume.load(); }
    void setMasterEnabled(bool enabled);
    bool isMasterEnabled() const { return m_masterEnabled.load(); }

    // Bypass mode
    void setBypass(bool bypass);
    bool isBypassed() const { return m_bypass.load(); }

    // State
    bool isRunning() const { return m_running.load(); }
    const AudioEngineConfig& getConfig() const { return m_config; }

signals:
    void audioLevelsChanged(float left, float right);
    void spectrumDataReady(const std::vector<float>& magnitudes);
    void deviceChanged(const QString& type, const QString& deviceId);
    void errorOccurred(const QString& error);
    void stateChanged(bool running);

private:
    void processAudio();
    void audioCallback(const AudioBuffer& input, AudioBuffer& output);
    void updateAnalysis(const AudioBuffer& buffer);
    void setupDefaultEffects();

    AudioEngineConfig m_config;

    std::unique_ptr<AudioCapture> m_capture;
    std::unique_ptr<AudioOutput> m_output;
    std::unique_ptr<EffectChain> m_effectChain;
    std::unique_ptr<DSPProcessor> m_dspProcessor;
    std::unique_ptr<SpectrumAnalyzer> m_spectrumAnalyzer;
    std::unique_ptr<MLEngine> m_mlEngine;

    std::unique_ptr<std::thread> m_processingThread;
    std::atomic<bool> m_running{false};
    std::atomic<bool> m_masterEnabled{true};
    std::atomic<float> m_masterVolume{1.0f};
    std::atomic<bool> m_bypass{false};

    mutable std::mutex m_mutex;

    // Lock-free ring buffer for audio data
    std::unique_ptr<LockFreeRingBuffer<float>> m_inputRingBuffer;
    std::unique_ptr<LockFreeRingBuffer<float>> m_outputRingBuffer;
};

} // namespace SoundBoost
