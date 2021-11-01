//
// Created by Jie Ren on 2021/10/12.
//

#ifndef THREADPOOL_SAFECALLEE_HPP
#define THREADPOOL_SAFECALLEE_HPP

#include <functional>

namespace ThreadPool {
class SafeCallee_t {
  std::function<void(bool)> fn_;

public:
  explicit SafeCallee_t(std::function<void(bool)> &&fn) : fn_(std::move(fn)){};

  SafeCallee_t(const std::function<void(bool)> &fn) = delete;

  SafeCallee_t(SafeCallee_t &&Callee) noexcept : fn_(std::move(Callee.fn_)){};

  SafeCallee_t(const SafeCallee_t &Callee) = delete;

  SafeCallee_t &operator=(SafeCallee_t &&Callee) noexcept {
    fn_ = std::move(Callee.fn_);
    return *this;
  }

  auto operator=(const SafeCallee_t &Callee) = delete;

  ~SafeCallee_t() {
    if (bool(fn_))
      fn_(true);
  }

  void operator()() {
    fn_(false);
    fn_ = std::function<void(bool)>{nullptr};
  }

  explicit operator bool() const noexcept { return bool(fn_); }
};
} // namespace ThreadPool

#endif // THREADPOOL_SAFECALLEE_HPP
