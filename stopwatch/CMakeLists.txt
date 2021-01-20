cmake_minimum_required(VERSION 2.6.0)
project(Stopwatch)

add_library(stopwatch INTERFACE)
target_include_directories(stopwatch INTERFACE "${CMAKE_CURRENT_LIST_DIR}/src")
