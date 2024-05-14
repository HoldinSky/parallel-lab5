cmake_minimum_required(VERSION 3.28)
project(lab5)

set(CMAKE_CXX_STANDARD 17)

link_directories(src/**)

add_executable(app
        src/main.cpp
        src/common.h
        src/pool/concurrent_queue.h
        src/pool/thread_pool.h
        src/pool/thread_pool.cpp
)
