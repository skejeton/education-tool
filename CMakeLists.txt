cmake_minimum_required(VERSION 3.20.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(catedu)

set(ADDITIONAL_FILES "")

if (APPLE)
    set(ADDITIONAL_FILES catedu/impl.m)
endif()

file(GLOB CATEDU_SRC_FILES "catedu/*.cpp" "catedu/*/*/*.cpp" "catedu/*/*/*/*.cpp" "catedu/*/*.cpp" ${ADDITIONAL_FILES} "lib/lib-enet/*.c")

if (${ENV_TYPE} MATCHES "UNITTEST")
    add_compile_definitions(RUN_TESTS)
    list(REMOVE_ITEM CATEDU_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/catedu/main.cpp)
    list(APPEND CATEDU_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/tests/unit/test_main.cpp)
endif()

if (${ENV_TYPE} MATCHES "TESTBED")
    list(REMOVE_ITEM CATEDU_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/catedu/main.cpp)
    list(APPEND CATEDU_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/tests/testbed/${TESTBED_NAME}/tb_main.cpp)
endif()

add_executable(${PROJECT_NAME} ${CATEDU_SRC_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

if (WIN32)
    add_compile_options(/fsanitize=address)
    target_link_libraries(${PROJECT_NAME} PUBLIC d3d11 ws2_32 winmm)
endif()

if (APPLE)
    find_library(QUARTZ_CORE QuartzCore)
    find_library(METAL Metal)
    find_library(METAL_KIT MetalKit)
    find_library(COCOA_LIBRARY Cocoa ONLY)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${COCOA_LIBRARY} ${METAL} ${METAL_KIT} ${QUARTZ_CORE})
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(${PROJECT_NAME} PUBLIC GL X11 pthread dl Xcursor Xi)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ./)
target_include_directories(${PROJECT_NAME} PUBLIC lib/)
target_include_directories(${PROJECT_NAME} PUBLIC lib/tau)
target_include_directories(${PROJECT_NAME} PUBLIC lib/lib-enet/include)
