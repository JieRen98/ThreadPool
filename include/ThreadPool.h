//
// Created by Jie Ren (jieren9806@gmail.com) on 2021/10/10.
//

#ifndef CPPTHREADPOOL_THREADPOOL_H
#define CPPTHREADPOOL_THREADPOOL_H

#include <functional>
#include <future>
#include <tuple>
#include <MyConcepts.h>
#include <TaskQueue.h>

namespace ThreadPool {
    class ThreadPool {
        TaskQueue_t queue_;

        template<typename Ret_t>
        struct SubmitHelper {
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
        struct SubmitHelper<void> {
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

    public:
        template<typename F, CP::IsSupportedPtr ...Args>
            auto Submit(F&& f, Args &&...args) {
            auto tuple = SubmitHelper<decltype(f(*args...))>::template call<F, Args...>(
                std::forward<F>(f),
                std::forward<Args>(args)...);
            queue_.emplace(std::move(std::get<1>(tuple)));
            return std::get<0>(tuple);
        };
    };
}

#endif //CPPTHREADPOOL_THREADPOOL_H
