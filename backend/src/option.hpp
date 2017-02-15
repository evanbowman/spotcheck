#pragma once

#include <array>
#include <cstdint>
#include <string>

template <typename T> class option {
    std::array<uint8_t, sizeof(T)> m_bytes;
    bool m_initialized;

public:
    option() : m_initialized(false) {}
    option(T && val) {
        m_initialized = true;
        *reinterpret_cast<T *>(m_bytes.data()) = std::forward<T>(val);
    }
    option(const option<T> & other) {
        if (other.m_initialized) {
            m_bytes = other.m_bytes;
            m_initialized = true;
        } else {
            m_initialized = false;
        }
    }
    option(option<T> && other) {
        if (other.m_initialized) {
            m_bytes = std::move(other.m_bytes);
            m_initialized = true;
            other.m_initialized = false;
        } else {
            m_initialized = false;
        }
    }
    void release() {
        if (m_initialized) {
            reinterpret_cast<T *>(m_bytes.data())->~T();
        }
        m_initialized = false;
    }
    option<T> & operator=(const option<T> & other) {
        m_bytes = other.m_bytes;
        m_initialized = other.m_initialized;
        return *this;
    }
    option<T> & operator=(option<T> && other) {
        m_bytes = std::move(other.m_bytes);
        m_initialized = other.m_initialized;
        other.m_initialized = false;
        return *this;
    }
    ~option() { release(); }
    operator bool() { return m_initialized; }
    const T & unwrap() const { return *(T *)m_bytes.data(); }
};
