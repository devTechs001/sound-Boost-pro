// src/core/ml/ModelLoader.hpp
#pragma once

#include <string>
#include <memory>

namespace SoundBoost {

class ModelLoader {
public:
    ModelLoader() = default;
    ~ModelLoader() = default;

    bool loadModel(const std::string& path);
    void* getModel() { return m_model; }

private:
    void* m_model{nullptr};
};

} // namespace SoundBoost
