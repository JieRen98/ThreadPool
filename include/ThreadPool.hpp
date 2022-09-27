//
// Created by Jie Ren (jieren9806@gmail.com) on 2021/10/10.
//

#ifndef CPPTHREADPOOL_THREADPOOL_HPP
#define CPPTHREADPOOL_THREADPOOL_HPP

#include <Common.h>
#include <MyConcepts.h>

#include <Worker.hpp>
#include <future>
#include <tuple>

namespace ThreadPool {
template <SubmitKind submitKind, typename Ret_t>
struct SubmitHelper;

template <typename Ret_t>
struct SubmitHelper<AutoPtr, Ret_t> {
  template <typename F, CP::IsSupportedPtr... Args>
  static auto call(F &&f, std::decay_t<Args>... args) {
    std::promise<Ret_t> promise{};
    auto future = promise.get_future();
    auto task = new std::packaged_task<void()>{
        [f = std::forward<F>(f), ... args = std::move(args),
         promise = std::move(promise)]() mutable -> void {
          promise.set_value(f(*args...));
        }};
    return std::make_tuple(std::move(future), task);
  }
};

template <>
struct SubmitHelper<AutoPtr, void> {
  template <typename F, CP::IsSupportedPtr... Args>
  static auto call(F &&f, std::decay_t<Args>... args) {
    std::promise<void> promise{};
    auto future = promise.get_future();
    auto task = new std::packaged_task<void()>{
        [f = std::forward<F>(f), ... args = std::move(args),
         promise = std::move(promise)]() mutable -> void {
          f(*args...);
          promise.set_value();
        }};
    return std::make_tuple(std::move(future), task);
  }
};

template <typename Ret_t>
struct SubmitHelper<Traditional, Ret_t> {
  template <typename F, typename... Args>
  static auto call(F &&f, Args &&...args) {
    std::promise<Ret_t> promise{};
    auto future = promise.get_future();
    auto task = new std::packaged_task<void()>{
        [f = std::forward<F>(f), ... args = std::forward<Args>(args),
         promise = std::move(promise)]() mutable -> void {
          promise.set_value(f(std::forward<Args>(args)...));
        }};
    return std::make_tuple(std::move(future), task);
  }
};

template <>
struct SubmitHelper<Traditional, void> {
  template <typename F, typename... Args>
  static auto call(F &&f, Args &&...args) {
    std::promise<void> promise{};
    auto future = promise.get_future();
    auto task = new std::packaged_task<void()>{
        [f = std::forward<F>(f), ... args = std::forward<Args>(args),
         promise = std::move(promise)]() mutable -> void {
          f(std::forward<Args>(args)...);
          promise.set_value();
        }};
    return std::make_tuple(std::move(future), task);
  }
};

template <SubmitKind submitKind>
struct ReturnTypeHelper;

template <>
struct ReturnTypeHelper<Traditional> {
  template <typename F, typename... Args>
  static auto call(F &&f, Args &&...args) {
    return f(std::forward<Args>(args)...);
  }
};

template <>
struct ReturnTypeHelper<AutoPtr> {
  template <typename F, CP::IsSupportedPtr... Args>
  static auto call(F &&f, Args &&...args) {
    return f((*args)...);
  }
};

ThreadPool::ThreadPool(const std::size_t world_size) : threads_{world_size} {}

template <SubmitKind submitKind, typename F, typename... Args>
auto ThreadPool::submit(F &&f, Args &&...args) {
  using Ret_t = decltype(ReturnTypeHelper<submitKind>::call(
      std::forward<F>(f), std::forward<Args>(args)...));
  auto tuple = SubmitHelper<submitKind, Ret_t>::template call<F, Args...>(
      std::forward<F>(f), std::forward<Args>(args)...);
  queue_.push(std::get<1>(tuple));
  cv_.notify_one();
  return std::move(std::get<0>(tuple));
}

void ThreadPool::start() {
  shutdown_flag_ = false;
  for (auto &thread : threads_) {
    thread = std::thread{Worker{this}};
  }
}

void ThreadPool::shutdown() {
  shutdown_flag_ = true;
  cv_.notify_all();
  for (auto &thread : threads_) {
    thread.join();
  }
  while (!queue_.empty()) {
    std::packaged_task<void()> *task;
    bool success = queue_.pop(task);
    if (success) {
      delete task;
    }
  }
}
}  // namespace ThreadPool

#endif  // CPPTHREADPOOL_THREADPOOL_HPP
