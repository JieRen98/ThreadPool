cmake_minimum_required(VERSION 3.20)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED)

add_library(ThreadPool INTERFACE)
target_include_directories(ThreadPool INTERFACE include)
target_link_libraries(ThreadPool INTERFACE Boost::boost)
target_compile_options(ThreadPool INTERFACE -pthread -O3)
