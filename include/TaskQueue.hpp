//
// Created by Jie Ren (jieren9806@gmail.com) on 2021/10/10.
//

#ifndef CPPTHREADPOOL_TASKQUEUE_HPP
#define CPPTHREADPOOL_TASKQUEUE_HPP

#include <Common.h>

namespace ThreadPool {
auto TaskQueue::push(std::function<void()> &&fn) {
  std::unique_lock<std::mutex> unique_lock{mutex_};
  return queue_.push(std::move(fn));
}

std::function<void ()> TaskQueue::popSafe() {
  std::unique_lock<std::mutex> unique_lock{mutex_};
  if (!queue_.empty()) {
    std::function<void ()> task_fn{std::move(queue_.front())};
    queue_.pop();
    return task_fn;
  } else {
    return std::function<void ()>{nullptr};
  }
}

bool TaskQueue::empty() const { return queue_.empty(); }
} // namespace ThreadPool

#endif // CPPTHREADPOOL_TASKQUEUE_HPP