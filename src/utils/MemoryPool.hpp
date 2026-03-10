// src/utils/MemoryPool.hpp
#pragma once

#include <vector>
#include <memory>
#include <mutex>

namespace SoundBoost {

template<typename T, size_t BlockSize = 256>
class MemoryPool {
public:
    MemoryPool() = default;
    ~MemoryPool() = default;

    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

    T* allocate() {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_freeList.empty()) {
            allocateBlock();
        }

        T* ptr = m_freeList.back();
        m_freeList.pop_back();
        return ptr;
    }

    void deallocate(T* ptr) {
        if (ptr) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_freeList.push_back(ptr);
        }
    }

    template<typename... Args>
    T* construct(Args&&... args) {
        T* ptr = allocate();
        new (ptr) T(std::forward<Args>(args)...);
        return ptr;
    }

    void destroy(T* ptr) {
        ptr->~T();
        deallocate(ptr);
    }

    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_blocks.clear();
        m_freeList.clear();
    }

private:
    void allocateBlock() {
        auto block = std::make_unique<char[]>(BlockSize * sizeof(T));
        T* ptr = reinterpret_cast<T*>(block.get());

        for (size_t i = 0; i < BlockSize; ++i) {
            m_freeList.push_back(&ptr[i]);
        }

        m_blocks.push_back(std::move(block));
    }

    std::vector<std::unique_ptr<char[]>> m_blocks;
    std::vector<T*> m_freeList;
    std::mutex m_mutex;
};

} // namespace SoundBoost
