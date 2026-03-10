// src/services/ServiceLocator.cpp
#include "ServiceLocator.hpp"
#include "../utils/Logger.hpp"

namespace SoundBoost {

ServiceLocator& ServiceLocator::getInstance() {
    static ServiceLocator instance;
    return instance;
}

void ServiceLocator::shutdownAll() {
    for (auto& [typeIndex, service] : m_services) {
        LOG_DEBUG("Shutting down service: {}", service->getName());
        service->shutdown();
    }
}

} // namespace SoundBoost
