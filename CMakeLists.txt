cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/buildtools/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")
set(VCPKG_TARGET_TRIPLET x64-windows-static CACHE STRING "Vcpkg target triplet")

project(lemon)

set(CPM_SOURCE_CACHE ${PROJECT_SOURCE_DIR}/.cpm-cache)
set(LEMON_CPM_CMAKE_PATH ${CMAKE_SOURCE_DIR}/buildtools/cmake/CPM.cmake)
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_subdirectory(lib/lemon-shared)
add_subdirectory(lib/lemon-shader) # temporary!
add_subdirectory(engine)
add_subdirectory(demo)
#add_subdirectory(tools)

#option(LEMON_BUILD_TESTS ON)
#if (LEMON_BUILD_TESTS)
#    add_subdirectory(test)
#endif ()
