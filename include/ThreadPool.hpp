//
// Created by Jie Ren (jieren9806@gmail.com) on 2021/10/10.
//

#ifndef CPPTHREADPOOL_THREADPOOL_HPP
#define CPPTHREADPOOL_THREADPOOL_HPP

#include <Common.h>
#include <MyConcepts.h>

#include <TaskQueue.hpp>
#include <Worker.hpp>
#include <future>
#include <tuple>

namespace ThreadPool {
template <typename Ret_t>
struct ThreadPool::SubmitHelper {
  template <typename F, CP::IsSupportedPtr... Args>
  static auto call(std::decay_t<F> f, std::decay_t<Args>... args) {
    auto promise = std::make_shared<std::promise<Ret_t>>();
    auto task_ptr = std::make_shared<std::packaged_task<void()>>(
        [f = std::move(f), ... args = std::move(args),
         promise = promise]() -> void { promise->set_value(f(*args...)); });
    std::function<void()> wrapped_fn{
        [task_ptr = std::move(task_ptr)]() { (*task_ptr)(); }};
    return std::make_tuple(std::move(promise), std::move(wrapped_fn));
  }
};

template <>
struct ThreadPool::SubmitHelper<void> {
  template <typename F, CP::IsSupportedPtr... Args>
  static auto call(std::decay_t<F> f, std::decay_t<Args>... args) {
    auto promise = std::make_shared<std::promise<void>>();
    auto task_ptr = std::make_shared<std::packaged_task<void()>>(
        [f = std::move(f), ... args = std::move(args),
         promise = promise]() -> void {
          f(*args...);
          promise->set_value();
        });
    std::function<void()> wrapped_fn{
        [task_ptr = std::move(task_ptr)]() { (*task_ptr)(); }};
    return std::make_tuple(std::move(promise), std::move(wrapped_fn));
  }
};

ThreadPool::ThreadPool(const std::size_t world_size) : threads_{world_size} {}

template <typename F, CP::IsSupportedPtr... Args>
auto ThreadPool::submit(F &&f, Args &&...args) {
  auto tuple = SubmitHelper<decltype(f(*args...))>::template call<F, Args...>(
      std::forward<F>(f), std::forward<Args>(args)...);
  queue_.push(std::move(std::get<1>(tuple)));
  cv_.notify_one();
  return std::move(std::get<0>(tuple));
}

void ThreadPool::start() {
  for (auto &thread : threads_) thread = std::thread{Worker{this}};
}

void ThreadPool::shutdown() {
  shutdown_flag_ = true;
  cv_.notify_all();
  for (auto &thread : threads_) {
    thread.join();
  }
}
}  // namespace ThreadPool

#endif  // CPPTHREADPOOL_THREADPOOL_HPP
