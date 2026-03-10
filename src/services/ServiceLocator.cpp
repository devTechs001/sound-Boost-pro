// src/services/ServiceLocator.cpp
#include "ServiceLocator.hpp"
#include "../utils/Logger.hpp"

namespace SoundBoost {

ServiceLocator::~ServiceLocator() {
    shutdownAll();
}

void ServiceLocator::initializeAll() {
    LOG_INFO("Initializing {} services...", m_orderedServices.size());

    for (auto& service : m_orderedServices) {
        if (!service->initialize()) {
            LOG_ERROR("Failed to initialize service: {}", service->getName());
            throw std::runtime_error("Service initialization failed: " +
                service->getName());
        }
        LOG_DEBUG("Initialized service: {}", service->getName());
    }
}

void ServiceLocator::shutdownAll() {
    LOG_INFO("Shutting down services...");

    // Shutdown in reverse order
    for (auto it = m_orderedServices.rbegin(); it != m_orderedServices.rend(); ++it) {
        (*it)->shutdown();
        LOG_DEBUG("Shutdown service: {}", (*it)->getName());
    }

    m_orderedServices.clear();
    m_services.clear();
}

} // namespace SoundBoost
