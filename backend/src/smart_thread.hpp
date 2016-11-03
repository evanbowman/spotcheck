#pragma once

#include <thread>

class smart_thread {
public:
    template<typename Function, typename ...Args>
    explicit smart_thread(Function && f, Args && ...args) :
        m_thread(std::forward<Function>(f), std::forward<Args>(args)...) {}
    smart_thread(std::thread && thread) : m_thread(std::move(thread)) {}
    smart_thread(const smart_thread &) = delete;
    smart_thread(smart_thread && other) { m_thread = std::move(other.m_thread); }
    smart_thread & operator=(smart_thread &) = delete;
    smart_thread & operator=(smart_thread && other) {
        m_thread = std::move(other.m_thread);
        return *this;
    }
    const std::thread & get() const { return m_thread; }
    std::thread && release() { return std::move(m_thread); }
    ~smart_thread() { if (m_thread.joinable()) m_thread.join(); }       
private:
    std::thread m_thread;
};
