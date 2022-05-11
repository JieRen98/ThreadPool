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

auto TaskQueue::pop() {
  std::unique_lock<std::mutex> unique_lock{mutex_};
  if (!queue_.empty()) {
    auto task_fn{std::move(queue_.front())};
    queue_.pop();
    return task_fn;
  } else {
    throw std::runtime_error("pop an empty queue");
  }
}

bool TaskQueue::empty() const { return queue_.empty(); }
} // namespace ThreadPool

#endif // CPPTHREADPOOL_TASKQUEUE_HPP