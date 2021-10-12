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
	ThreadPool_t::Worker_t::Worker_t(ThreadPool_t* tp) : tp_(tp) {}

	void ThreadPool_t::Worker_t::operator()() noexcept {
		std::function<void()> fn{ tp_->queue_.pop() };
		while (!shutdown_flag_) {
            if (bool(fn))
                fn();
            {
				std::unique_lock<std::mutex> unique_lock(tp_->cv_mutex_);
                tp_->cv_.wait(unique_lock);
                fn = tp_->queue_.pop();
            }
		}
	}

    void ThreadPool_t::Worker_t::ShutDown() { shutdown_flag_ = true; }
}

#endif // CPPTHREADPOOL_WORKER_H