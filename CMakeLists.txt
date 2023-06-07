cmake_minimum_required(VERSION 3.20.0)

project(catedu)

set(ADDITIONAL_FILES "")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(ADDITIONAL_FILES src/impl.m)
endif()

file(GLOB CATEDU_SRC_FILES "src/*.cpp" ${ADDITIONAL_FILES} lib/imgui/imgui.cpp lib/imgui/imgui_draw.cpp lib/imgui/imgui_demo.cpp lib/imgui/imgui_widgets.cpp lib/imgui/imgui_tables.cpp)

add_executable(catedu ${CATEDU_SRC_FILES})

set_property(TARGET catedu PROPERTY CXX_STANDARD 17)

if (APPLE) # or if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(QUARTZ_CORE QuartzCore)
    find_library(METAL Metal)
    find_library(METAL_KIT MetalKit)
    find_library(COCOA_LIBRARY Cocoa ONLY)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${COCOA_LIBRARY} ${METAL} ${METAL_KIT} ${QUARTZ_CORE})
endif()

target_include_directories(catedu PUBLIC lib/)
