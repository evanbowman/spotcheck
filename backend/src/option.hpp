#pragma once

#include <array>
#include <string>

template <typename T> class option {
    std::array<uint8_t, sizeof(T)> m_bytes;
    bool m_initialized;

public:
    option() : m_initialized(false) {}
    option(const T && val) { *reinterpret_cast<T *>(m_bytes.data()) = val; }
    template <typename... Args> explicit option(Args &&... args) {
        *reinterpret_cast<T *>(m_bytes.data()) = T(args...);
        m_initialized = true;
    }
    explicit option(const option<T> & other) {
        if (other.m_initialized) {
            m_bytes = other.m_bytes;
            m_initialized = true;
        } else {
            m_initialized = false;
        }
    }
    explicit option(option<T> && other) {
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
    // const T & unwrap() const {
    //     if (!m_initialized) throw null_opt_err();
    //     return *(T *)m_bytes.data();
    // }
};
