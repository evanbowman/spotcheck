#pragma once

#include <iostream>
#include <chrono>
#include <queue>
#include <mutex>

#include "smart_thread.hpp"

namespace core {
	class work_queue {
	public:
		explicit work_queue(const size_t pool_size);
		work_queue(const work_queue &) = delete;
		work_queue(work_queue &&) = delete;
		work_queue & operator=(const work_queue &) = delete;
		work_queue & operator=(work_queue &&) = delete;
		void submit(std::function<void()> && work);
		~work_queue();
	private:
		bool m_is_running;
		std::mutex m_queue_mtx;
		std::vector<smart_thread> m_pool;
		std::queue<std::function<void()>> m_queue;
	};
}
