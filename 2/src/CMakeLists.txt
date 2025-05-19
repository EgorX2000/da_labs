cmake_minimum_required(VERSION 3.10)

project("lab_2")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(main main.cpp)
