#include "work_queue.hpp"

namespace core {
	work_queue::work_queue(const size_t pool_size) : m_is_running(true) {
		for (int i = 0; i < pool_size; i += 1) {
			m_pool.emplace_back([&]() {
			    while (m_is_running) {
					std::function<void()> work_routine;
					{
						std::lock_guard<std::mutex> queue_grd(m_queue_mtx);
						if (!m_queue.empty()) {
							work_routine = m_queue.front();
							m_queue.pop();
						}
					}
					if (work_routine != nullptr) {
						work_routine();
					} else {
						std::this_thread::sleep_for(std::chrono::milliseconds(50));
					}
				}
			});
		}
	}

	void work_queue::submit(std::function<void()> && work) {
		std::lock_guard<std::mutex> queue_grd(m_queue_mtx);
		m_queue.push(work);
	}

	work_queue::~work_queue() {
		m_is_running = false;
		for (auto & thread : m_pool) {
			thread.join();
		}
	}
}
