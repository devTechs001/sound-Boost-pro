// src/core/ml/GenreDetector.hpp
#pragma once

#include <string>
#include <vector>
#include <utility>

namespace SoundBoost {

class GenreDetector {
public:
    GenreDetector() = default;
    ~GenreDetector() = default;

    bool loadModel(const std::string& modelPath) { return true; }
    bool isLoaded() const { return m_loaded; }

    std::pair<std::string, float> detect(const std::vector<float>& features) {
        return {"Unknown", 0.0f};
    }

    const std::vector<std::string>& getGenreLabels() const { return m_genreLabels; }

private:
    std::vector<std::string> m_genreLabels = {
        "Blues", "Classical", "Country", "Disco", "Hip-Hop",
        "Jazz", "Metal", "Pop", "Reggae", "Rock",
        "Electronic", "Folk", "R&B", "Soul", "Punk"
    };
    bool m_loaded{false};
};

} // namespace SoundBoost
