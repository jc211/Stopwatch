cmake_minimum_required(VERSION 2.6.0)
project(StopwatchTester)

add_executable(stopwatch_tester 
  "${CMAKE_CURRENT_LIST_DIR}/src/stopwatch_tester.cpp"
)

target_link_libraries(stopwatch_tester PRIVATE stopwatch)

if (WIN32)
    target_link_libraries(stopwatch_tester 
                        PRIVATE
                        ws2_32)
endif()