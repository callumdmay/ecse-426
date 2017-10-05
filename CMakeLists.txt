cmake_minimum_required(VERSION 3.8)

project(ecse-426)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(BUILD_1 main)
set(SOURCE_FILES_1 Lab1/Sources/main.c)
add_executable(${BUILD_1} ${SOURCE_FILES_1})