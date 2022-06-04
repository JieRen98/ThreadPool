//
// Created by Jie Ren (jieren9806@gmail.com) on 2021/10/11.
//

#ifndef CPPTHREADPOOL_WORKER_HPP
#define CPPTHREADPOOL_WORKER_HPP

#include <Common.h>

#include <functional>

namespace ThreadPool {
ThreadPool::Worker::Worker(ThreadPool *tp) : tp_(tp) {}

void ThreadPool::Worker::operator()() noexcept {
  while (!tp_->shutdown_flag_) {
    auto fn = tp_->queue_.popSafe();
    if (!fn.valid()) {
      std::unique_lock<std::mutex> unique_lock(tp_->cv_mutex_);
      tp_->cv_.wait(unique_lock);
      continue;
    }
    fn();
  }
}
}  // namespace ThreadPool

#endif  // CPPTHREADPOOL_WORKER_HPP