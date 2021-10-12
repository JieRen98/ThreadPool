//
// Created by Jie Ren (jieren9806@gmail.com) on 2021/10/10.
//

#ifndef CPPTHREADPOOL_TASKQUEUE_H
#define CPPTHREADPOOL_TASKQUEUE_H

#include <Common.h>

namespace ThreadPool {
	auto TaskQueue_t::emplace(std::function<void(bool)>&& fn) {
		std::unique_lock<std::mutex> unique_lock{ mutex_ };
		return queue_.push(SafeCallee{ std::move(fn) });
	}

    auto TaskQueue_t::emplace(SafeCallee&& fn) {
        std::unique_lock<std::mutex> unique_lock{ mutex_ };
        return queue_.push(std::move(fn));
    }

    SafeCallee TaskQueue_t::pop() {
		std::unique_lock<std::mutex> unique_lock{ mutex_ };
		if (!queue_.empty()) {
			SafeCallee task_fn{ std::move(queue_.front()) };
			queue_.pop();
			return task_fn;
		}
		else {
			return SafeCallee { std::function<void(bool)>{ nullptr } };
		}
	}
}

#endif // CPPTHREADPOOL_TASKQUEUE_H