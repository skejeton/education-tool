cmake_minimum_required(VERSION 3.20.0)

project(catedu)

set(ADDITIONAL_FILES "")

if (APPLE)
    set(ADDITIONAL_FILES src/impl.m)
endif()

file(GLOB CATEDU_SRC_FILES "src/*.cpp" ${ADDITIONAL_FILES} lib/imgui/imgui.cpp lib/imgui/imgui_draw.cpp lib/imgui/imgui_demo.cpp lib/imgui/imgui_widgets.cpp lib/imgui/imgui_tables.cpp)

if (RUN_TESTS EQUAL YES)
    add_compile_definitions(RUN_TESTS)
    list(REMOVE_ITEM CATEDU_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
    list(APPEND CATEDU_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_main.cpp)
endif()

message(${CATEDU_SRC_FILES})

add_executable(${PROJECT_NAME} ${CATEDU_SRC_FILES} src/easy_gui.cpp src/easy_gui.hpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC d3d11)
endif()

if (APPLE)
    find_library(QUARTZ_CORE QuartzCore)
    find_library(METAL Metal)
    find_library(METAL_KIT MetalKit)
    find_library(COCOA_LIBRARY Cocoa ONLY)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${COCOA_LIBRARY} ${METAL} ${METAL_KIT} ${QUARTZ_CORE})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC lib/)
target_include_directories(${PROJECT_NAME} PUBLIC lib/tau)
