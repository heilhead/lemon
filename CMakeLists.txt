cmake_minimum_required(VERSION 3.20)

project(lemon)

set(CPM_SOURCE_CACHE ${PROJECT_SOURCE_DIR}/.cpm-cache)
set(LEMON_CPM_CMAKE_PATH ${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

option(LEMON_BUILD_TESTS OFF)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(engine)
add_subdirectory(demo)

if (LEMON_BUILD_TESTS)
    add_subdirectory(test)
endif ()

unset(LEMON_BUILD_TESTS)