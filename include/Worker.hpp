//
// Created by Jie Ren (jieren9806@gmail.com) on 2021/10/11.
//

#ifndef CPPTHREADPOOL_WORKER_HPP
#define CPPTHREADPOOL_WORKER_HPP

#include <Common.h>
#include <condition_variable>
#include <functional>
#include <thread>
#include <vector>

namespace ThreadPool {
ThreadPool::Worker::Worker(ThreadPool *tp) : tp_(tp) {}

void ThreadPool::Worker::operator()() noexcept {
  std::function<void ()> &&fn = tp_->queue_.popSafe();
  while (!tp_->shutdown_flag_) {
    if (bool(fn)) {
      fn();
      fn = nullptr;
    }
    {
      std::unique_lock<std::mutex> unique_lock(tp_->cv_mutex_);
      tp_->cv_.wait(unique_lock);
      fn = tp_->queue_.popSafe();
    }
  }
}
} // namespace ThreadPool

#endif // CPPTHREADPOOL_WORKER_HPP