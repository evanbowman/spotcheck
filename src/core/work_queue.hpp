#pragma once

#include <iostream>
#include <chrono>
#include <thread>
#include <queue>
#include <mutex>

namespace core {
	class work_queue {
	public:
		explicit work_queue(const size_t pool_size);
		void submit(std::function<void()> && work);
		~work_queue();
	private:
		bool m_is_running;
		std::mutex m_queue_mtx;
		std::vector<std::thread> m_pool;
		std::queue<std::function<void()>> m_queue;
	};
}
