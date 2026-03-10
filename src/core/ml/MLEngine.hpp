// src/core/ml/MLEngine.hpp
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <mutex>

namespace SoundBoost {

class AudioBuffer;
class ModelLoader;
class FeatureExtractor;
class GenreDetector;
class BeatDetector;
class VoiceDetector;
class AdaptiveEQ;

struct MLAnalysisResult {
    std::string detectedGenre;
    float genreConfidence{0.0f};

    float bpm{0.0f};
    std::vector<float> beatPositions;

    bool hasVoice{false};
    float voiceConfidence{0.0f};

    std::vector<float> suggestedEQ;

    // Audio characteristics
    float brightness{0.0f};
    float warmth{0.0f};
    float dynamicRange{0.0f};
    float bassContent{0.0f};
};

class MLEngine {
public:
    MLEngine();
    ~MLEngine();

    // Initialization
    bool loadModels(const std::string& modelPath);
    bool isInitialized() const { return m_initialized; }

    // Analysis
    MLAnalysisResult analyze(const AudioBuffer& buffer);

    // Individual analysis components
    std::string detectGenre(const AudioBuffer& buffer);
    float detectBPM(const AudioBuffer& buffer);
    bool detectVoice(const AudioBuffer& buffer);
    std::vector<float> suggestEQ(const AudioBuffer& buffer);

    // Adaptive processing
    void enableAdaptiveProcessing(bool enable);
    bool isAdaptiveProcessingEnabled() const { return m_adaptiveEnabled; }

    // Real-time analysis callback
    using AnalysisCallback = std::function<void(const MLAnalysisResult&)>;
    void setAnalysisCallback(AnalysisCallback callback);

    // Processing
    void processRealtime(const AudioBuffer& buffer);

private:
    void extractFeatures(const AudioBuffer& buffer);
    void runInference();
    void postProcess();

    std::unique_ptr<ModelLoader> m_modelLoader;
    std::unique_ptr<FeatureExtractor> m_featureExtractor;
    std::unique_ptr<GenreDetector> m_genreDetector;
    std::unique_ptr<BeatDetector> m_beatDetector;
    std::unique_ptr<VoiceDetector> m_voiceDetector;
    std::unique_ptr<AdaptiveEQ> m_adaptiveEQ;

    bool m_initialized{false};
    bool m_adaptiveEnabled{false};

    AnalysisCallback m_analysisCallback;
    std::mutex m_mutex;

    // Feature cache
    std::vector<float> m_mfccFeatures;
    std::vector<float> m_spectralFeatures;
    std::vector<float> m_temporalFeatures;
};

} // namespace SoundBoost
