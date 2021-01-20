cmake_minimum_required(VERSION 2.6.0)
project(StopwatchMonitor)

find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)

set(IMGUI_SRC 
    "${CMAKE_CURRENT_LIST_DIR}/src/imgui/imgui.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/imgui/imgui_demo.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/imgui/imgui_draw.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/imgui/imgui_impl_glfw.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/imgui/imgui_impl_opengl3.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/imgui/imgui_tables.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/imgui/imgui_widgets.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/imgui/implot.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/imgui/implot_demo.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/imgui/implot_items.cpp"
)

add_executable(stopwatch_monitor
  "${CMAKE_CURRENT_LIST_DIR}/src/gui/window.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/main.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/server.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/application.cpp"
  ${IMGUI_SRC}
)

target_link_libraries(stopwatch_monitor 
                      PUBLIC
                      GLEW::GLEW
                      glfw)
if (WIN32)
    target_link_libraries(stopwatch_monitor 
                        PUBLIC
                        ws2_32)
endif()

