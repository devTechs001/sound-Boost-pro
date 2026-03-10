// src/services/ServiceLocator.hpp
#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>
#include <vector>

namespace SoundBoost {

class IService {
public:
    virtual ~IService() = default;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual std::string getName() const = 0;
};

class ServiceLocator {
public:
    ServiceLocator() = default;
    ~ServiceLocator();

    // Non-copyable
    ServiceLocator(const ServiceLocator&) = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;

    template<typename T>
    void registerService(std::shared_ptr<T> service) {
        static_assert(std::is_base_of_v<IService, T>,
            "T must derive from IService");

        std::type_index typeIndex(typeid(T));
        m_services[typeIndex] = service;
        m_orderedServices.push_back(service);
    }

    template<typename T>
    T& get() {
        std::type_index typeIndex(typeid(T));
        auto it = m_services.find(typeIndex);

        if (it == m_services.end()) {
            throw std::runtime_error("Service not found: " +
                std::string(typeid(T).name()));
        }

        return *std::static_pointer_cast<T>(it->second);
    }

    template<typename T>
    T* tryGet() {
        std::type_index typeIndex(typeid(T));
        auto it = m_services.find(typeIndex);

        if (it == m_services.end()) {
            return nullptr;
        }

        return std::static_pointer_cast<T>(it->second).get();
    }

    template<typename T>
    bool has() const {
        return m_services.find(std::type_index(typeid(T))) != m_services.end();
    }

    std::vector<std::shared_ptr<IService>> getAllServices() const {
        return m_orderedServices;
    }

    void initializeAll();
    void shutdownAll();

private:
    std::unordered_map<std::type_index, std::shared_ptr<IService>> m_services;
    std::vector<std::shared_ptr<IService>> m_orderedServices;
};

} // namespace SoundBoost
