cmake_minimum_required(VERSION 3.10)

project(lemon)

include(cmake/CPM.cmake)

set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

option(LEMON_BuildTests OFF)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(engine)
add_subdirectory(demo)

if (LEMON_BuildTests)
    add_subdirectory(test)
endif ()
