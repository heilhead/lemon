cmake_minimum_required(VERSION 3.20)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(LEMON_ROOT "../..")
set(CMAKE_TOOLCHAIN_FILE ${LEMON_ROOT}/buildtools/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")
set(VCPKG_TARGET_TRIPLET x64-windows-static CACHE STRING "Vcpkg target triplet")

project(lemon-shader)

set(CPM_SOURCE_CACHE ${LEMON_ROOT}/.cpm-cache)
set(LEMON_CPM_CMAKE_PATH ${LEMON_ROOT}/buildtools/cmake/CPM.cmake)

# Tint currently has issues when compiled using c++20.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(SOURCE_FILES
    "src/lemon/shader/ShaderReflection.cpp"
    "src/lemon/shader/reflection.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
add_library(lemon::shader ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        PRIVATE
        ${PROJECT_SOURCE_DIR}/src)

set(TINT_BUILD_SAMPLES OFF)
set(TINT_BUILD_DOCS OFF)
set(TINT_BUILD_SPV_READER OFF)
set(TINT_BUILD_HLSL_WRITER OFF)
set(TINT_BUILD_MSL_WRITER OFF)
set(TINT_BUILD_SPV_WRITER OFF)
set(TINT_BUILD_WGSL_WRITER OFF)
set(TINT_BUILD_TESTS OFF)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/tint)

find_package(magic_enum CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC libtint PRIVATE lemon::shared magic_enum::magic_enum)
