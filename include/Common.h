//
// Created by Jie Ren (jieren9806@gmail.com) on 2021/10/11.
//

#ifndef CPPTHREADPOOL_COMMON_H
#define CPPTHREADPOOL_COMMON_H

#include <MyConcepts.h>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace ThreadPool {
class TaskQueue {
  std::queue<std::function<void()>> queue_;
  mutable std::mutex mutex_;

public:
  auto push(std::function<void()> &&fn);

  auto pop();

  bool empty() const;
};

class ThreadPool {
  class Worker {
    ThreadPool *tp_;

  public:
    explicit Worker(ThreadPool *tp);

    void operator()() noexcept;
  };

  bool shutdown_flag_{false};
  std::vector<std::thread> threads_;
  std::thread dispatcher_thread_;
  std::condition_variable cv_{};
  std::mutex cv_mutex_{};
  TaskQueue queue_{};

  friend Worker;

  template <typename Ret_t> struct SubmitHelper;

  struct Dispatcher {
    ThreadPool *tp_;

    explicit Dispatcher(ThreadPool *tp);

    void operator()() const;
  };

public:
  explicit ThreadPool(std::size_t world_size);

  template <typename F, CP::IsSupportedPtr... Args>
  auto submit(F &&f, Args &&...args);

  void start();

  void shutdown();
};
} // namespace ThreadPool

#endif // CPPTHREADPOOL_COMMON_H