// src/core/ml/GenreDetector.hpp
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <array>

namespace Ort { class Session; class Env; }

namespace SoundBoost {

class GenreDetector {
public:
    GenreDetector();
    ~GenreDetector();

    bool loadModel(const std::string& modelPath);
    bool isLoaded() const { return m_loaded; }

    std::pair<std::string, float> detect(const std::vector<float>& features);
    std::vector<std::pair<std::string, float>> detectTop(
        const std::vector<float>& features, int topK = 3);

    const std::vector<std::string>& getGenreLabels() const { return m_genreLabels; }

private:
    std::vector<float> runInference(const std::vector<float>& features);
    void initializeLabels();

    std::unique_ptr<Ort::Env> m_env;
    std::unique_ptr<Ort::Session> m_session;

    std::vector<std::string> m_genreLabels;
    std::vector<int64_t> m_inputShape;
    std::vector<int64_t> m_outputShape;

    bool m_loaded{false};
};

} // namespace SoundBoost
