# SoundBoost Pro - API Reference

## Core Audio API

### AudioEngine

```cpp
class AudioEngine : public QObject {
public:
    bool initialize(const AudioEngineConfig& config);
    void start();
    void stop();
    
    // Device management
    std::vector<AudioDevice> getInputDevices() const;
    std::vector<AudioDevice> getOutputDevices() const;
    bool setInputDevice(const std::string& deviceId);
    bool setOutputDevice(const std::string& deviceId);
    
    // Effect chain
    EffectChain& getEffectChain();
    
    // ML Engine
    MLEngine& getMLEngine();
    
    // Master controls
    void setMasterVolume(float volume);
    void setBypass(bool bypass);
    
signals:
    void audioLevelsChanged(float left, float right);
    void spectrumDataReady(const std::vector<float>& magnitudes);
};
```

### AudioBuffer

```cpp
class AudioBuffer {
public:
    AudioBuffer(size_t numSamples, int channels, int sampleRate);
    
    // Data access
    float* getData();
    float* getChannelData(int channel);
    
    // Properties
    size_t getNumSamples() const;
    int getNumChannels() const;
    int getSampleRate() const;
    
    // Operations
    void clear();
    void applyGain(float gain);
    void normalize(float targetPeak = 1.0f);
    
    // Analysis
    float getPeak() const;
    float getRMS() const;
    std::pair<float, float> getPeakLevels() const;
};
```

## DSP API

### DSPProcessor

```cpp
class DSPProcessor {
public:
    void initialize(int sampleRate, int bufferSize);
    
    // Filter operations
    void applyLowPass(AudioBuffer& buffer, float cutoffHz, float Q = 0.707f);
    void applyHighPass(AudioBuffer& buffer, float cutoffHz, float Q = 0.707f);
    void applyBandPass(AudioBuffer& buffer, float centerHz, float bandwidth);
    void applyPeakEQ(AudioBuffer& buffer, float centerHz, float gainDb, float Q);
    void applyLowShelf(AudioBuffer& buffer, float cutoffHz, float gainDb, float slope);
    void applyHighShelf(AudioBuffer& buffer, float cutoffHz, float gainDb, float slope);
    
    // FFT operations
    std::vector<float> computeMagnitudeSpectrum(const AudioBuffer& buffer);
    
    // Dynamics
    void applyCompression(AudioBuffer& buffer, float threshold, float ratio,
                         float attack, float release);
    void applyLimiting(AudioBuffer& buffer, float ceiling);
};
```

### BiquadFilter

```cpp
class BiquadFilter {
public:
    void setCoefficients(const BiquadCoefficients& coeffs);
    float process(float input);
    void processBlock(float* data, size_t numSamples);
    void reset();
};

struct BiquadCoefficients {
    float b0{1.0f}, b1{0.0f}, b2{0.0f};
    float a1{0.0f}, a2{0.0f};
};
```

## Effects API

### EffectBase

```cpp
class EffectBase {
public:
    virtual void initialize(int sampleRate, int bufferSize) = 0;
    virtual void process(AudioBuffer& buffer) = 0;
    virtual void reset() = 0;
    
    virtual std::string getName() const = 0;
    virtual std::string getCategory() const = 0;
    
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
    void setParameter(const std::string& name, float value);
    float getParameter(const std::string& name) const;
};
```

### BassBooster

```cpp
class BassBooster : public EffectBase {
public:
    void setFrequency(float frequency);    // 20-200 Hz
    void setGain(float gainDb);            // 0-18 dB
    void setWidth(float width);            // 0.5-4.0 Q
    void setHarmonics(float amount);       // 0-100%
    void setSubBass(bool enabled);
};
```

### Equalizer

```cpp
class Equalizer : public EffectBase {
public:
    void setBandGain(int band, float gainDb);    // -24 to +24 dB
    void setBandFrequency(int band, float freq); // Hz
    void setBandQ(int band, float Q);            // 0.1-10
    
    void setFlat();
    void setBassBoost();
    void setVShape();
    void setVocalBoost();
};
```

## ML API

### MLEngine

```cpp
class MLEngine {
public:
    bool loadModels(const std::string& modelPath);
    
    MLAnalysisResult analyze(const AudioBuffer& buffer);
    
    std::string detectGenre(const AudioBuffer& buffer);
    float detectBPM(const AudioBuffer& buffer);
    bool detectVoice(const AudioBuffer& buffer);
    std::vector<float> suggestEQ(const AudioBuffer& buffer);
    
    void enableAdaptiveProcessing(bool enable);
    
    using AnalysisCallback = std::function<void(const MLAnalysisResult&)>;
    void setAnalysisCallback(AnalysisCallback callback);
};

struct MLAnalysisResult {
    std::string detectedGenre;
    float genreConfidence;
    float bpm;
    std::vector<float> beatPositions;
    bool hasVoice;
    float voiceConfidence;
    std::vector<float> suggestedEQ;
    float brightness;
    float warmth;
    float dynamicRange;
    float bassContent;
};
```

## Service API

### ServiceLocator

```cpp
class ServiceLocator {
public:
    template<typename T>
    void registerService(std::shared_ptr<T> service);
    
    template<typename T>
    T& get();
    
    template<typename T>
    T* tryGet();
    
    template<typename T>
    bool has();
};
```

### SettingsService

```cpp
class SettingsService : public QObject, public IService {
    Q_OBJECT
public:
    template<typename T>
    T get(const QString& key, const T& defaultValue = T{});
    
    template<typename T>
    void set(const QString& key, const T& value);
    
    void save();
    void reset();
    
signals:
    void settingChanged(const QString& key, const QVariant& value);
};
```

### ProfileService

```cpp
class ProfileService : public QObject, public IService {
    Q_OBJECT
public:
    std::vector<QString> getProfileNames() const;
    AudioProfile getProfile(const QString& name) const;
    bool saveProfile(const AudioProfile& profile);
    void loadProfile(const QString& name);
    
    bool exportProfile(const QString& name, const QString& path) const;
    bool importProfile(const QString& path);
    
signals:
    void profileLoaded(const QString& name);
    void profilesChanged();
};
```

## Utility API

### Logger

```cpp
class Logger {
public:
    static Logger& getInstance();
    
    void initialize(LogLevel level = LogLevel::Info,
                   const std::string& logFile = "");
    
    template<typename... Args>
    void info(std::string_view format, Args&&... args);
    
    template<typename... Args>
    void debug(std::string_view format, Args&&... args);
    
    template<typename... Args>
    void error(std::string_view format, Args&&... args);
};

// Convenience macros
#define LOG_INFO(...) SoundBoost::Logger::getInstance().info(__VA_ARGS__)
#define LOG_DEBUG(...) SoundBoost::Logger::getInstance().debug(__VA_ARGS__)
#define LOG_ERROR(...) SoundBoost::Logger::getInstance().error(__VA_ARGS__)
```

### Configuration

```cpp
class Configuration {
public:
    static Configuration& getInstance();
    
    bool load(const std::string& path = "");
    bool save(const std::string& path = "");
    
    template<typename T>
    T get(const std::string& key, const T& defaultValue = T{});
    
    template<typename T>
    void set(const std::string& key, const T& value);
};
```
