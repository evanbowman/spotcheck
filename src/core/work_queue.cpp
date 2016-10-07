#include "work_queue.hpp"

namespace core {
	work_queue::work_queue(const size_t pool_size) : m_is_running(true) {
		for (int i = 0; i < pool_size; i += 1) {
			m_pool.emplace_back([this]() {
			    while (this->m_is_running) {
					std::function<void()> work_routine;
					{
						std::lock_guard<std::mutex> queue_grd(this->m_queue_mtx);
						if (!this->m_queue.empty()) {
							work_routine = this->m_queue.front();
							this->m_queue.pop();
						}
					}
					if (work_routine != nullptr) {
						work_routine();
					} else {
						// Otherwise the threads will constantly poll for work
						// and eat up the cpu usage. When there's no work available,
						// go to sleep for a while.
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
	}
}
