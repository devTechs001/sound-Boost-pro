// src/services/EventBus.hpp
#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <any>

namespace SoundBoost {

using EventCallback = std::function<void(const std::any&)>;

class EventBus {
public:
    static EventBus& getInstance();

    using SubscriptionId = size_t;

    SubscriptionId subscribe(const std::string& eventName, EventCallback callback);
    void unsubscribe(SubscriptionId id);
    void unsubscribe(const std::string& eventName, SubscriptionId id);

    void publish(const std::string& eventName, const std::any& data = {});

    void clear();

private:
    EventBus() = default;
    ~EventBus() = default;

    struct Subscription {
        std::string eventName;
        EventCallback callback;
    };

    std::unordered_map<SubscriptionId, Subscription> m_subscriptions;
    std::unordered_map<std::string, std::vector<SubscriptionId>> m_eventSubscribers;
    std::mutex m_mutex;
    SubscriptionId m_nextId{1};
};

} // namespace SoundBoost
