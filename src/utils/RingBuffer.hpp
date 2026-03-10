// src/utils/RingBuffer.hpp
#pragma once

#include <vector>
#include <atomic>
#include <cstddef>

namespace SoundBoost {

template<typename T>
class RingBuffer {
public:
    explicit RingBuffer(size_t capacity)
        : m_buffer(capacity)
        , m_head(0)
        , m_tail(0)
        , m_size(0)
    {}

    bool write(const T& value) {
        if (m_size.load() >= m_buffer.size()) {
            return false; // Buffer full
        }

        m_buffer[m_head] = value;
        m_head = (m_head + 1) % m_buffer.size();
        m_size.fetch_add(1);
        return true;
    }

    bool read(T& value) {
        if (m_size.load() == 0) {
            return false; // Buffer empty
        }

        value = m_buffer[m_tail];
        m_tail = (m_tail + 1) % m_buffer.size();
        m_size.fetch_sub(1);
        return true;
    }

    size_t write(const T* data, size_t count) {
        size_t written = 0;
        for (size_t i = 0; i < count; ++i) {
            if (write(data[i])) {
                ++written;
            } else {
                break;
            }
        }
        return written;
    }

    size_t read(T* data, size_t count) {
        size_t read = 0;
        for (size_t i = 0; i < count; ++i) {
            T value;
            if (read(value)) {
                data[i] = value;
                ++read;
            } else {
                break;
            }
        }
        return read;
    }

    void clear() {
        m_head = 0;
        m_tail = 0;
        m_size = 0;
    }

    size_t size() const { return m_size.load(); }
    size_t capacity() const { return m_buffer.size(); }
    bool empty() const { return m_size.load() == 0; }
    bool full() const { return m_size.load() >= m_buffer.size(); }

private:
    std::vector<T> m_buffer;
    std::atomic<size_t> m_head;
    std::atomic<size_t> m_tail;
    std::atomic<size_t> m_size;
};

} // namespace SoundBoost
