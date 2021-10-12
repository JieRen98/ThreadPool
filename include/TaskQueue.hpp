//
// Created by Jie Ren (jieren9806@gmail.com) on 2021/10/10.
//

#ifndef CPPTHREADPOOL_TASKQUEUE_H
#define CPPTHREADPOOL_TASKQUEUE_H

#include <Common.h>

namespace ThreadPool {
	template<typename Fn_t>
	auto TaskQueue_t::emplace(Fn_t&& fn) {
		std::unique_lock<std::mutex> unique_lock{ mutex_ };
		return queue_.emplace(std::forward<Fn_t>(fn));
	};

	std::function<void()> TaskQueue_t::pop() {
		std::unique_lock<std::mutex> unique_lock{ mutex_ };
		if (!queue_.empty()) {
			std::function<void()>&& task_fn = std::move(queue_.front());
			queue_.pop();
			return task_fn;
		}
		else {
			return std::function<void()> { nullptr };
		}
	};
}

#endif // CPPTHREADPOOL_TASKQUEUE_H