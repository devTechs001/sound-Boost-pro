// src/utils/LockFreeQueue.hpp
#pragma once

#include <atomic>
#include <memory>
#include <optional>

namespace SoundBoost {

template<typename T>
class LockFreeQueue {
public:
    LockFreeQueue() = default;

    void enqueue(const T& value) {
        auto newNode = std::make_unique<Node>(value);

        Node* oldTail = m_tail.load(std::memory_order_relaxed);
        Node* newTail = newNode.get();

        while (!m_tail.compare_exchange_weak(oldTail, newTail,
               std::memory_order_release, std::memory_order_relaxed)) {
            // oldTail is updated by compare_exchange_weak
        }

        oldTail->next.store(newNode.release(), std::memory_order_release);
    }

    std::optional<T> dequeue() {
        Node* oldHead = m_head.load(std::memory_order_relaxed);

        while (oldHead != m_tail.load(std::memory_order_acquire)) {
            Node* newHead = oldHead->next.load(std::memory_order_acquire);

            if (m_head.compare_exchange_weak(oldHead, newHead,
                    std::memory_order_release, std::memory_order_relaxed)) {
                T value = oldHead->data;
                delete oldHead;
                return value;
            }
        }

        return std::nullopt;
    }

    bool empty() const {
        return m_head.load(std::memory_order_relaxed) ==
               m_tail.load(std::memory_order_relaxed);
    }

private:
    struct Node {
        T data;
        std::atomic<Node*> next{nullptr};

        Node(const T& value) : data(value) {}
    };

    Node m_dummyNode{};  // Sentinel node
    std::atomic<Node*> m_head{&m_dummyNode};
    std::atomic<Node*> m_tail{&m_dummyNode};
};

} // namespace SoundBoost
