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
            std::function<void(bool)> wrapped_fn = [f = std::move(f), ...args = std::move(args), promise = promise](bool destruct) mutable -> void {
                if (destruct) {
                    (args.reset(), ...);
                    promise.reset();
                } else {
                    promise->set_value(f(*args...));
                    (args.reset(), ...);
                }
            };
            return std::make_tuple(std::move(promise), std::move(wrapped_fn));
        }
    };

    template<>
    struct ThreadPool_t::SubmitHelper<void> {
        template<typename F, CP::IsSupportedPtr ...Args>
        static auto call(std::decay_t<F> f, std::decay_t<Args> ...args) {
            auto promise = std::make_shared<std::promise<void >>();
            std::function<void(bool)> wrapped_fn = [f = std::move(f), ...args = std::move(args), promise = promise](bool destruct) mutable -> void {
                if (destruct) {
                    (args.reset(), ...);
                    promise.reset();
                } else {
                    f(*args...);
                    (args.reset(), ...);
                    promise->set_value();
                }
            };
            return std::make_tuple(std::move(promise), std::move(wrapped_fn));
        }
    };

    ThreadPool_t::ThreadPool_t(const std::size_t world_size) : threads_{ world_size }{}

    template<typename F, CP::IsSupportedPtr ...Args>
    auto ThreadPool_t::Submit(F&& f, Args &&...args) {
        auto tuple = SubmitHelper<decltype(f(*args...))>::template call<F, Args...>(
            std::forward<F>(f),
            std::forward<Args>(args)...);
        queue_.push(std::move(std::get<1>(tuple)));
        cv_.notify_one();
        return std::get<0>(tuple);
    }

    void ThreadPool_t::start() {
        for (auto& thread : threads_)
            thread = std::thread{ Worker_t{this} };
    }

    void ThreadPool_t::shutdown() {
        shutdown_flag_ = true;
        cv_.notify_all();
        for (auto& thread : threads_) {
            while (!thread.joinable())
                cv_.notify_all();
            thread.join();
        }
    }
}

#endif // CPPTHREADPOOL_THREADPOOL_H
