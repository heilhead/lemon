cmake_minimum_required(VERSION 3.10)

project(lemon)

include(cmake/CPM.cmake)

add_subdirectory(engine)
add_subdirectory(demo)
add_subdirectory(test)
