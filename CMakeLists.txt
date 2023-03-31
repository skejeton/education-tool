cmake_minimum_required(VERSION 3.20.0)

project(catedu)

file(GLOB CATEDU_SRC_FILES "src/*.cpp" lib/imgui/imgui.cpp lib/imgui/imgui_draw.cpp lib/imgui/imgui_demo.cpp lib/imgui/imgui_widgets.cpp lib/imgui/imgui_tables.cpp)

add_executable(catedu ${CATEDU_SRC_FILES})

target_include_directories(catedu PUBLIC lib/)