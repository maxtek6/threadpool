cmake_minimum_required(VERSION 3.0)

project(threadpool)

option(THREADPOOL_TESTS "Build test suite" off)
option(THREADPOOL_COVER "Check code coverage" off)

add_library(threadpool_a STATIC threadpool.cpp)
target_include_directories(threadpool_a PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(threadpool SHARED threadpool.cpp)
target_include_directories(threadpool PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(THREADPOOL_TESTS)
    add_subdirectory(tests)
endif()