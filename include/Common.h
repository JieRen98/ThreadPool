//
// Created by Jie Ren (jieren9806@gmail.com) on 2021/10/11.
//

#ifndef CPPTHREADPOOL_COMMON_H
#define CPPTHREADPOOL_COMMON_H

#include <mutex>
#include <queue>
#include <functional>
#include <MyConcepts.h>
#include <SafeCallee.hpp>

namespace ThreadPool {
    class TaskQueue_t {
        std::queue<SafeCallee_t> queue_;
        mutable std::mutex mutex_;

    public:
        auto push(std::function<void(bool)>&& fn);

        auto push(SafeCallee_t&& fn);

        SafeCallee_t pop();
    };

    class ThreadPool_t {
        class Worker_t {
            ThreadPool_t* tp_;

        public:
            explicit Worker_t(ThreadPool_t* tp);

            void operator()() noexcept;
        };

        bool shutdown_flag_{ false };
        std::vector<std::thread> threads_;
        std::condition_variable cv_{};
        std::mutex cv_mutex_{};
        TaskQueue_t queue_{};

        friend Worker_t;

        template<typename Ret_t>
        struct SubmitHelper;

    public:
        explicit ThreadPool_t(std::size_t world_size);

        template<typename F, CP::IsSupportedPtr ...Args>
        auto Submit(F&& f, Args &&...args);

        void start();

        void shutdown();
    };
}

#endif // CPPTHREADPOOL_COMMON_H