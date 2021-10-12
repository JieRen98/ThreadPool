//
// Created by Jie Ren (jieren9806@gmail.com) on 2021/10/11.
//

#ifndef CPPTHREADPOOL_WORKER_H
#define CPPTHREADPOOL_WORKER_H

#include <thread>
#include <vector>
#include <condition_variable>
#include <functional>
#include <Common.h>

namespace ThreadPool {
	ThreadPool_t::Worker_t::Worker_t(ThreadPool_t* tp) : tp_(tp) {};

	void ThreadPool_t::Worker_t::operator()() {
		std::function<void()> fn;
		while (!shutdown_flag_) {
			{
				std::unique_lock<std::mutex> unique_lock(tp_->cv_mutex_);
				if (!bool(fn))
					tp_->cv_.wait(unique_lock);
				fn = tp_->queue_.pop();
			}
			if (!bool(fn))
				fn();
		}
	};
};

#endif // CPPTHREADPOOL_WORKER_H