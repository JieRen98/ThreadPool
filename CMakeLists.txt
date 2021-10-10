cmake_minimum_required(VERSION 3.20)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    add_definitions(-DDEBUG=1)
    add_subdirectory(test)
else()
    add_definitions(-DDEBUG=0)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -O3")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
