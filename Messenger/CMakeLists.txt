cmake_minimum_required(VERSION 3.10.2)
project(messenger)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set (CMAKE_CXX_FLAGS "-pthread")
configure_file(messenger.hpp messenger.hpp)
add_executable(messenger messenger.cpp)

