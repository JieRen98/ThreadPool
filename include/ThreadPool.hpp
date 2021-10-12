//
// Created by Jie Ren (jieren9806@gmail.com) on 2021/10/10.
//

#ifndef CPPTHREADPOOL_THREADPOOL_H
#define CPPTHREADPOOL_THREADPOOL_H

#include <future>
#include <tuple>
#include <Common.h>
#include <MyConcepts.h>
#include <TaskQueue.hpp>
#include <Worker.hpp>

namespace ThreadPool {
    template<typename Ret_t>
    struct ThreadPool_t::SubmitHelper {
        template<typename F, CP::IsSupportedPtr ...Args>
        static auto call(std::decay_t<F> f, std::decay_t<Args> ...args) {
            auto promise = std::make_shared<std::promise<Ret_t >>();
            auto wrapped_fn = [f = std::move(f), ...args = std::move(args), promise = promise]() mutable -> void {
                promise->set_value(f(*args...));
                (args.reset(), ...);
                promise.reset();
            };
            return std::make_tuple(std::move(promise), std::move(wrapped_fn));
        }
    };

    template<>
    struct ThreadPool_t::SubmitHelper<void> {
        template<typename F, CP::IsSupportedPtr ...Args>
        static auto call(std::decay_t<F> f, std::decay_t<Args> ...args) {
            auto promise = std::make_shared<std::promise<void >>();
            auto wrapped_fn = [f = std::move(f), ...args = std::move(args), promise = promise]() mutable -> void {
                f(*args...);
                promise->set_value();
                (args.reset(), ...);
                promise.reset();
            };
            return std::make_tuple(std::move(promise), std::move(wrapped_fn));
        }
    };

    ThreadPool_t::ThreadPool_t(const std::size_t world_size) : threads_{ world_size }{};

    template<typename F, CP::IsSupportedPtr ...Args>
    auto ThreadPool_t::Submit(F&& f, Args &&...args) {
        auto tuple = SubmitHelper<decltype(f(*args...))>::template call<F, Args...>(
            std::forward<F>(f),
            std::forward<Args>(args)...);
        queue_.emplace(std::move(std::get<1>(tuple)));
        cv_.notify_one();
        return std::get<0>(tuple);
    };

    void ThreadPool_t::start() {
        for (auto& thread : threads_)
            thread = std::thread{ Worker_t{this} };
    }
}

#endif // CPPTHREADPOOL_THREADPOOL_H
