// src/core/ml/MLEngine.cpp
#include "MLEngine.hpp"
#include "../audio/AudioBuffer.hpp"
#include "../../utils/Logger.hpp"

#include <algorithm>

namespace SoundBoost {

MLEngine::MLEngine()
    : m_modelLoader(std::make_unique<ModelLoader>())
    , m_featureExtractor(std::make_unique<FeatureExtractor>())
    , m_genreDetector(std::make_unique<GenreDetector>())
    , m_beatDetector(std::make_unique<BeatDetector>())
    , m_voiceDetector(std::make_unique<VoiceDetector>())
    , m_adaptiveEQ(std::make_unique<AdaptiveEQ>())
{
}

MLEngine::~MLEngine() = default;

bool MLEngine::loadModels(const std::string& modelPath) {
    LOG_INFO("Loading ML models from: {}", modelPath);

    // In production, load actual ONNX models
    // For now, initialize components
    m_featureExtractor->initialize(48000, 2048, 512);
    m_adaptiveEQ->initialize(48000);

    m_initialized = true;
    LOG_INFO("ML engine initialized (models would be loaded from {})", modelPath);
    return true;
}

MLAnalysisResult MLEngine::analyze(const AudioBuffer& buffer) {
    std::lock_guard<std::mutex> lock(m_mutex);

    MLAnalysisResult result;

    if (!m_initialized) {
        return result;
    }

    // Extract features
    extractFeatures(buffer);

    // Genre detection
    auto [genre, confidence] = m_genreDetector->detect(m_mfccFeatures);
    result.detectedGenre = genre;
    result.genreConfidence = confidence;

    // BPM detection
    result.bpm = m_beatDetector->detectBPM(buffer);

    // Voice detection
    auto [hasVoice, voiceConf] = m_voiceDetector->detect(m_spectralFeatures);
    result.hasVoice = hasVoice;
    result.voiceConfidence = voiceConf;

    // Suggest EQ settings
    result.suggestedEQ = m_adaptiveEQ->suggestSettings(
        m_spectralFeatures, genre, hasVoice);

    // Calculate audio characteristics
    result.brightness = m_featureExtractor->calculateBrightness();
    result.warmth = m_featureExtractor->calculateWarmth();
    result.dynamicRange = m_featureExtractor->calculateDynamicRange();
    result.bassContent = m_featureExtractor->calculateBassContent();

    return result;
}

void MLEngine::extractFeatures(const AudioBuffer& buffer) {
    // Extract MFCC features
    m_mfccFeatures = m_featureExtractor->extractMFCC(buffer, 13);

    // Extract spectral features
    m_spectralFeatures = m_featureExtractor->extractSpectralFeatures(buffer);

    // Extract temporal features
    m_temporalFeatures = m_featureExtractor->extractTemporalFeatures(buffer);
}

std::string MLEngine::detectGenre(const AudioBuffer& buffer) {
    if (!m_initialized) return "Unknown";

    auto features = m_featureExtractor->extractMFCC(buffer, 13);
    auto [genre, confidence] = m_genreDetector->detect(features);
    return genre;
}

float MLEngine::detectBPM(const AudioBuffer& buffer) {
    if (!m_initialized) return 0.0f;
    return m_beatDetector->detectBPM(buffer);
}

bool MLEngine::detectVoice(const AudioBuffer& buffer) {
    if (!m_initialized) return false;

    auto features = m_featureExtractor->extractSpectralFeatures(buffer);
    auto [hasVoice, confidence] = m_voiceDetector->detect(features);
    return hasVoice;
}

std::vector<float> MLEngine::suggestEQ(const AudioBuffer& buffer) {
    if (!m_initialized) return std::vector<float>(10, 0.0f);

    auto spectralFeatures = m_featureExtractor->extractSpectralFeatures(buffer);
    auto mfccFeatures = m_featureExtractor->extractMFCC(buffer, 13);

    auto [genre, _] = m_genreDetector->detect(mfccFeatures);
    auto [hasVoice, __] = m_voiceDetector->detect(spectralFeatures);

    return m_adaptiveEQ->suggestSettings(spectralFeatures, genre, hasVoice);
}

void MLEngine::enableAdaptiveProcessing(bool enable) {
    m_adaptiveEnabled = enable;
}

void MLEngine::setAnalysisCallback(AnalysisCallback callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_analysisCallback = std::move(callback);
}

void MLEngine::processRealtime(const AudioBuffer& buffer) {
    if (!m_adaptiveEnabled || !m_initialized) return;

    auto result = analyze(buffer);

    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_analysisCallback) {
        m_analysisCallback(result);
    }
}

void MLEngine::runInference() {
    // ML inference would happen here
}

void MLEngine::postProcess() {
    // Post-processing of ML results
}

} // namespace SoundBoost
