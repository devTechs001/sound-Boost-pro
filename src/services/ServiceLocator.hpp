// src/services/ServiceLocator.hpp
#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <typeindex>
#include <vector>

namespace SoundBoost {

class IService {
public:
    virtual ~IService() = default;
    virtual std::string getName() const = 0;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
};

class ServiceLocator {
public:
    static ServiceLocator& getInstance();

    template<typename T>
    void registerService(std::shared_ptr<T> service) {
        static_assert(std::is_base_of<IService, T>::value, "T must inherit from IService");
        m_services[typeid(T)] = service;
    }

    template<typename T>
    T& get() {
        auto it = m_services.find(typeid(T));
        if (it == m_services.end()) {
            throw std::runtime_error("Service not found: " + std::string(typeid(T).name()));
        }
        return *std::static_pointer_cast<T>(it->second);
    }

    template<typename T>
    const T& get() const {
        auto it = m_services.find(typeid(T));
        if (it == m_services.end()) {
            throw std::runtime_error("Service not found: " + std::string(typeid(T).name()));
        }
        return *std::static_pointer_cast<const T>(it->second);
    }

    template<typename T>
    bool hasService() const {
        return m_services.find(typeid(T)) != m_services.end();
    }

    const std::unordered_map<std::type_index, std::shared_ptr<IService>>& getAllServices() const {
        return m_services;
    }

    void shutdownAll();

private:
    ServiceLocator() = default;
    ~ServiceLocator() = default;
    ServiceLocator(const ServiceLocator&) = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;

    std::unordered_map<std::type_index, std::shared_ptr<IService>> m_services;
};

} // namespace SoundBoost
