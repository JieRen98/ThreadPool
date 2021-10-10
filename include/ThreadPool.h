//
// Created by Jie Ren (jieren9806@gmail.com) on 2021/10/10.
//

#ifndef CPPTHREADPOOL_THREADPOOL_H
#define CPPTHREADPOOL_THREADPOOL_H

#include <queue>
#include <functional>
#include <future>
#include <tuple>

namespace ThreadPool {
    namespace {
        template<typename T>
        struct IsSharedPtrHelper { constexpr static const bool value = false; };

        template<typename Value_t>
        struct IsSharedPtrHelper<std::shared_ptr<Value_t>> { constexpr static const bool value = true; };

        template<typename T>
        struct IsUniquePtrHelper { constexpr static const bool value = false; };

        template<typename Value_t, typename T_Deleter_t>
        struct IsUniquePtrHelper<std::unique_ptr<Value_t, T_Deleter_t>> { constexpr static const bool value = true; };

        template<typename T>
        concept IsSharedPtr = IsSharedPtrHelper<std::decay_t<T >>::value;

        template<typename T>
        concept IsUniquePtr = IsUniquePtrHelper<std::decay_t<T >> ::value;

        template<typename T>
        concept IsSupportedPtr = IsSharedPtr<T> || IsUniquePtr<T>;

        template<typename ...T>
        concept AllAreSupportedPtrs = (... && IsSupportedPtr<T>);
    }

    class ThreadPool {
        using Queue_t = std::queue<std::function<void()>>;
        Queue_t queue_;

        template<typename Ret_t>
        struct SubmitHelper {
            template<typename F, typename ...Args>
            requires AllAreSupportedPtrs<Args...>
            static auto submit(std::decay_t<F> f, std::decay_t<Args> ...args) {
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
            template<typename F, typename ...Args>
            requires AllAreSupportedPtrs<Args...>
            static auto submit(std::decay_t<F> f, std::decay_t<Args> ...args) {
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
        template<typename F, typename ...Args>
        requires AllAreSupportedPtrs<Args...>
        auto Submit(F &&f, Args &&...args) {
            auto tuple = SubmitHelper<decltype(f(*args...))>::template submit<F, Args...>(
                    std::forward<F>(f),
                    std::forward<Args>(args)...);
            queue_.template emplace(std::move(std::get<1>(tuple)));
            return std::get<0>(tuple);
        };
    };
}

#endif //CPPTHREADPOOL_THREADPOOL_H
