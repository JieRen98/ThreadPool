//
// Created by Jie Ren (jieren9806@gmail.com) on 2021/10/11.
//

#ifndef CPPTHREADPOOL_COMMON_H
#define CPPTHREADPOOL_COMMON_H

#include <MyConcepts.h>
#include <SafeCallee.hpp>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace ThreadPool {
class TaskQueue_t {
  std::queue<SafeCallee_t> queue_;
  mutable std::mutex mutex_;

public:
  auto push(std::function<void(bool)> &&fn);

  auto push(SafeCallee_t &&fn);

  auto pop();

  bool empty() const;
};

class ThreadPool_t {
  class Worker_t {
    ThreadPool_t *tp_;

  public:
    explicit Worker_t(ThreadPool_t *tp);

    void operator()() noexcept;
  };

  bool shutdown_flag_{false};
  std::vector<std::thread> threads_;
  std::thread dispatcher_thread_;
  std::condition_variable cv_{};
  std::mutex cv_mutex_{};
  TaskQueue_t queue_{};

  friend Worker_t;

  template <typename Ret_t> struct SubmitHelper;

  struct Dispatcher {
    ThreadPool_t *tp_;

    explicit Dispatcher(ThreadPool_t *tp);

    void operator()() const;
  };

public:
  explicit ThreadPool_t(std::size_t world_size);

  template <typename F, CP::IsSupportedPtr... Args>
  auto Submit(F &&f, Args &&...args);

  void start();

  void shutdown();
};
} // namespace ThreadPool

#endif // CPPTHREADPOOL_COMMON_H