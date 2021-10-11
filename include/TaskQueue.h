//
// Created by Jie Ren (jieren9806@gmail.com) on 2021/10/10.
//

#ifndef CPPTHREADPOOL_TASKQUEUE_H
#define CPPTHREADPOOL_TASKQUEUE_H

#include <mutex>
#include <queue>

namespace ThreadPool {
	class TaskQueue_t {
		std::queue<std::function<void()>> queue_;
		mutable std::mutex mutex_;

	public:
		template<typename Fn_t>
		auto emplace(Fn_t&& fn) {
			std::unique_lock<std::mutex> unique_lock{ mutex_ };
			return queue_.emplace(std::forward<Fn_t>(fn));
		}

		auto pop() {
			std::unique_lock<std::mutex> unique_lock{ mutex_ };
			std::function<void()> &&task_fn = std::move(queue_.front());
			queue_.pop();
			unique_lock.unlock();
			return task_fn;
		}
	};
}

#endif CPPTHREADPOOL_TASKQUEUE_H