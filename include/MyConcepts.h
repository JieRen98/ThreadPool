//
// Created by Jie Ren (jieren9806@gmail.com) on 2021/10/11.
//

#ifndef CPPTHREADPOOL_MYCONCEPTS_HPP
#define CPPTHREADPOOL_MYCONCEPTS_HPP

#include <memory.h>

namespace ThreadPool {
    namespace CP{
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
    }
}

#endif // CPPTHREADPOOL_MYCONCEPTS_HPP