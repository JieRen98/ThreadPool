cmake_minimum_required(VERSION 3.20)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED)

add_library(ThreadPool INTERFACE)
target_include_directories(ThreadPool INTERFACE include)
target_link_libraries(ThreadPool INTERFACE Boost::boost)
target_compile_options(ThreadPool INTERFACE -pthread)

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    add_definitions(-DDEBUG=1)
else ()
    add_definitions(-DDEBUG=0)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif ()
