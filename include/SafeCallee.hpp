//
// Created by renjie on 2021/10/12.
//

#ifndef THREADPOOL_SAFECALLEE_HPP
#define THREADPOOL_SAFECALLEE_HPP

#include <functional>

namespace ThreadPool {
    class SafeCallee {
        std::function<void(bool)> fn_;

    public:
        explicit SafeCallee(std::function<void(bool)> &&fn) : fn_(std::move(fn)) {};

        SafeCallee(const std::function<void(bool)> &fn) = delete;

        SafeCallee(SafeCallee &&Callee) noexcept : fn_(std::move(Callee.fn_)) {};

        SafeCallee(const SafeCallee &Callee) = delete;

        SafeCallee &operator=(SafeCallee &&Callee) noexcept { fn_ = std::move(Callee.fn_); return *this; }

        auto operator=(const SafeCallee &Callee) = delete;

        ~SafeCallee() { if (bool(fn_)) fn_(true); }

        void operator()() { fn_(false); }

        explicit operator bool() const noexcept { return bool(fn_); }
    };
}

#endif //THREADPOOL_SAFECALLEE_HPP
