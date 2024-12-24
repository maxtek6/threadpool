cmake_minimum_required(VERSION 3.10)

project(threadpool)

option(THREADPOOL_TESTS "Build test suite" off)
option(THREADPOOL_COVER "Check code coverage" off)
option(THREADPOOL_DOCS "Build docs" off)

add_library(threadpool INTERFACE)
target_include_directories(threadpool INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if(THREADPOOL_DOCS)
    add_subdirectory(docs)
endif()

if(THREADPOOL_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

add_subdirectory(example)