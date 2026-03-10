// src/core/ml/GenreDetector.cpp
#include "GenreDetector.hpp"
#include "../../utils/Logger.hpp"

#ifdef ENABLE_ML
#include <onnxruntime_cxx_api.h>
#endif

#include <algorithm>
#include <numeric>

namespace SoundBoost {

GenreDetector::GenreDetector() {
    initializeLabels();

#ifdef ENABLE_ML
    m_env = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "GenreDetector");
#endif
}

GenreDetector::~GenreDetector() = default;

void GenreDetector::initializeLabels() {
    m_genreLabels = {
        "Blues", "Classical", "Country", "Disco", "Hip-Hop",
        "Jazz", "Metal", "Pop", "Reggae", "Rock",
        "Electronic", "Folk", "R&B", "Soul", "Punk"
    };
}

bool GenreDetector::loadModel(const std::string& modelPath) {
#ifdef ENABLE_ML
    try {
        Ort::SessionOptions sessionOptions;
        sessionOptions.SetIntraOpNumThreads(2);
        sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

        m_session = std::make_unique<Ort::Session>(*m_env, modelPath.c_str(), sessionOptions);

        // Get input shape
        auto inputInfo = m_session->GetInputTypeInfo(0);
        auto tensorInfo = inputInfo.GetTensorTypeAndShapeInfo();
        m_inputShape = tensorInfo.GetShape();

        // Get output shape
        auto outputInfo = m_session->GetOutputTypeInfo(0);
        auto outputTensorInfo = outputInfo.GetTensorTypeAndShapeInfo();
        m_outputShape = outputTensorInfo.GetShape();

        m_loaded = true;
        LOG_INFO("Genre detector model loaded: {}", modelPath);
        return true;

    } catch (const Ort::Exception& e) {
        LOG_ERROR("Failed to load genre model: {}", e.what());
        return false;
    }
#else
    LOG_WARN("ML support not enabled");
    return false;
#endif
}

std::vector<float> GenreDetector::runInference(const std::vector<float>& features) {
#ifdef ENABLE_ML
    if (!m_loaded) {
        return std::vector<float>(m_genreLabels.size(), 1.0f / m_genreLabels.size());
    }

    try {
        Ort::AllocatorWithDefaultOptions allocator;

        // Prepare input tensor
        auto memoryInfo = Ort::MemoryInfo::CreateCpu(
            OrtArenaAllocator, OrtMemTypeDefault);

        std::vector<int64_t> inputShape = {1, static_cast<int64_t>(features.size())};

        Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
            memoryInfo, const_cast<float*>(features.data()), features.size(),
            inputShape.data(), inputShape.size());

        // Get input/output names
        auto inputName = m_session->GetInputNameAllocated(0, allocator);
        auto outputName = m_session->GetOutputNameAllocated(0, allocator);

        const char* inputNames[] = {inputName.get()};
        const char* outputNames[] = {outputName.get()};

        // Run inference
        auto outputTensors = m_session->Run(
            Ort::RunOptions{nullptr},
            inputNames, &inputTensor, 1,
            outputNames, 1);

        // Get output
        float* outputData = outputTensors[0].GetTensorMutableData<float>();
        size_t outputSize = outputTensors[0].GetTensorTypeAndShapeInfo().GetElementCount();

        return std::vector<float>(outputData, outputData + outputSize);

    } catch (const Ort::Exception& e) {
        LOG_ERROR("Inference error: {}", e.what());
        return std::vector<float>(m_genreLabels.size(), 1.0f / m_genreLabels.size());
    }
#else
    return std::vector<float>(m_genreLabels.size(), 1.0f / m_genreLabels.size());
#endif
}

std::pair<std::string, float> GenreDetector::detect(const std::vector<float>& features) {
    auto probabilities = runInference(features);

    if (probabilities.empty()) {
        return {"Unknown", 0.0f};
    }

    // Apply softmax
    float maxVal = *std::max_element(probabilities.begin(), probabilities.end());
    float sum = 0.0f;
    for (float& p : probabilities) {
        p = std::exp(p - maxVal);
        sum += p;
    }
    for (float& p : probabilities) {
        p /= sum;
    }

    // Find max
    auto maxIt = std::max_element(probabilities.begin(), probabilities.end());
    int maxIdx = std::distance(probabilities.begin(), maxIt);

    if (maxIdx < static_cast<int>(m_genreLabels.size())) {
        return {m_genreLabels[maxIdx], *maxIt};
    }

    return {"Unknown", 0.0f};
}

std::vector<std::pair<std::string, float>> GenreDetector::detectTop(
    const std::vector<float>& features, int topK) {

    auto probabilities = runInference(features);

    // Apply softmax
    float maxVal = *std::max_element(probabilities.begin(), probabilities.end());
    float sum = 0.0f;
    for (float& p : probabilities) {
        p = std::exp(p - maxVal);
        sum += p;
    }
    for (float& p : probabilities) {
        p /= sum;
    }

    // Create index-probability pairs
    std::vector<std::pair<int, float>> indexed;
    for (size_t i = 0; i < probabilities.size(); ++i) {
        indexed.emplace_back(i, probabilities[i]);
    }

    // Sort by probability
    std::partial_sort(indexed.begin(), indexed.begin() + topK, indexed.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });

    // Build result
    std::vector<std::pair<std::string, float>> result;
    for (int i = 0; i < topK && i < static_cast<int>(indexed.size()); ++i) {
        int idx = indexed[i].first;
        if (idx < static_cast<int>(m_genreLabels.size())) {
            result.emplace_back(m_genreLabels[idx], indexed[i].second);
        }
    }

    return result;
}

} // namespace SoundBoost
