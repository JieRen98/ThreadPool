//
// Created by Jie Ren (jieren9806@gmail.com) on 2021/10/10.
//

#ifndef CPPTHREADPOOL_TASKQUEUE_HPP
#define CPPTHREADPOOL_TASKQUEUE_HPP

#include <Common.h>

namespace ThreadPool {
	auto TaskQueue_t::push(std::function<void(bool)>&& fn) {
		std::unique_lock<std::mutex> unique_lock{ mutex_ };
		return queue_.push(SafeCallee_t{std::move(fn) });
	}

    auto TaskQueue_t::push(SafeCallee_t&& fn) {
        std::unique_lock<std::mutex> unique_lock{ mutex_ };
        return queue_.push(std::move(fn));
    }

    SafeCallee_t TaskQueue_t::pop() {
		std::unique_lock<std::mutex> unique_lock{ mutex_ };
		if (!queue_.empty()) {
			SafeCallee_t task_fn{std::move(queue_.front()) };
			queue_.pop();
			return task_fn;
		}
		else {
			return SafeCallee_t {std::function<void(bool)>{nullptr } };
		}
	}
}

#endif // CPPTHREADPOOL_TASKQUEUE_HPP