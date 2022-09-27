//
// Created by Jie Ren (jieren9806@gmail.com) on 2021/10/11.
//

#ifndef CPPTHREADPOOL_WORKER_HPP
#define CPPTHREADPOOL_WORKER_HPP

#include <common.h>

namespace ThreadPool {
ThreadPool::Worker::Worker(ThreadPool *tp) : tp_{tp} {}

void ThreadPool::Worker::operator()() noexcept {
  while (!tp_->shutdown_flag_) {
    if (!tp_->queue_.empty()) {
      std::packaged_task<void()> *fn;
      bool success = tp_->queue_.pop(fn);
      if (success) {
        (*fn)();
        delete fn;
      }
    } else {
      std::unique_lock<std::mutex> unique_lock{tp_->cv_mutex_};
      tp_->cv_.wait(unique_lock);
    }
  }
}
}  // namespace ThreadPool

#endif  // CPPTHREADPOOL_WORKER_HPP