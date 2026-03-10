// src/services/EventBus.cpp
#include "EventBus.hpp"
#include "../utils/Logger.hpp"

namespace SoundBoost {

EventBus& EventBus::getInstance() {
    static EventBus instance;
    return instance;
}

EventBus::SubscriptionId EventBus::subscribe(const std::string& eventName, EventCallback callback) {
    std::lock_guard<std::mutex> lock(m_mutex);

    SubscriptionId id = m_nextId++;

    Subscription sub{eventName, std::move(callback)};
    m_subscriptions[id] = std::move(sub);
    m_eventSubscribers[eventName].push_back(id);

    LOG_DEBUG("Subscribed to event '{}', id={}", eventName, id);
    return id;
}

void EventBus::unsubscribe(SubscriptionId id) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_subscriptions.find(id);
    if (it != m_subscriptions.end()) {
        const std::string& eventName = it->second.eventName;
        m_subscriptions.erase(it);

        auto& subscribers = m_eventSubscribers[eventName];
        subscribers.erase(
            std::remove(subscribers.begin(), subscribers.end(), id),
            subscribers.end()
        );

        LOG_DEBUG("Unsubscribed from event '{}', id={}", eventName, id);
    }
}

void EventBus::unsubscribe(const std::string& eventName, SubscriptionId id) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_subscriptions.find(id);
    if (it != m_subscriptions.end() && it->second.eventName == eventName) {
        m_subscriptions.erase(it);

        auto& subscribers = m_eventSubscribers[eventName];
        subscribers.erase(
            std::remove(subscribers.begin(), subscribers.end(), id),
            subscribers.end()
        );

        LOG_DEBUG("Unsubscribed from event '{}', id={}", eventName, id);
    }
}

void EventBus::publish(const std::string& eventName, const std::any& data) {
    std::vector<EventCallback> callbacks;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_eventSubscribers.find(eventName);
        if (it != m_eventSubscribers.end()) {
            for (SubscriptionId id : it->second) {
                auto subIt = m_subscriptions.find(id);
                if (subIt != m_subscriptions.end()) {
                    callbacks.push_back(subIt->second.callback);
                }
            }
        }
    }

    for (auto& callback : callbacks) {
        try {
            callback(data);
        } catch (const std::exception& e) {
            LOG_ERROR("Event callback exception for '{}': {}", eventName, e.what());
        }
    }
}

void EventBus::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_subscriptions.clear();
    m_eventSubscribers.clear();
}

} // namespace SoundBoost
