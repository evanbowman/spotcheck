#pragma once

#include <exception>
#include <string>

namespace core {
	class null_opt_err : public std::exception {
		std::string msg;
	public:
		null_opt_err() : msg("Attempt to access empty option!") {}
		inline const char * what() const noexcept override {
			return msg.c_str();
		}
		virtual ~null_opt_err() {}
	};
	
	template <typename T>
	class option {
		std::array<uint8_t, sizeof(T)> m_bytes;
		bool m_initialized;
	public:
		option() : m_initialized(false) {}
		option(const T && val) {
			*reinterpret_cast<T *>(m_bytes.data()) = val;
		}
		template <typename ...Args>
		option(Args && ...args) {
			*reinterpret_cast<T *>(m_bytes.data()) = T(args...);
			m_initialized = true;
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
		~option() {
			if (m_initialized) {
				reinterpret_cast<T *>(m_bytes.data())->~T();
			}
		}
		operator bool() { return m_initialized; }
		const T & value() const {
			if (!m_initialized) throw null_opt_err();
			return *(T *)m_bytes.data();
		}
	};
}
