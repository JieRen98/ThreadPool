//
// Created by Jie Ren (jieren9806@gmail.com) on 2021/10/11.
//

#ifndef CPPTHREADPOOL_COMMON_H
#define CPPTHREADPOOL_COMMON_H

#include <MyConcepts.h>

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

namespace ThreadPool {
class TaskQueue {
  using element = std::packaged_task<void()>;
  std::queue<element> queue_;
  mutable std::mutex mutex_;

 public:
  auto push(element &&fn);

  auto popSafe();

  bool empty() const;
};

enum SubmitKind { Traditional, AutoPtr };

class ThreadPool {
  class Worker {
    ThreadPool *tp_;

   public:
    explicit Worker(ThreadPool *tp);

    void operator()() noexcept;
  };

  bool shutdown_flag_{true};
  std::vector<std::thread> threads_;
  std::condition_variable cv_{};
  std::mutex cv_mutex_{};
  TaskQueue queue_{};

  friend Worker;

 public:
  explicit ThreadPool(std::size_t world_size);

  template <SubmitKind submitKind = AutoPtr, typename F, typename... Args>
  auto submit(F &&f, Args &&...args);

  void start();

  void shutdown();
};
}  // namespace ThreadPool

#endif  // CPPTHREADPOOL_COMMON_H