project(lemon-dawn)

set(SOURCE_FILES src/webgpu_cpp.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
add_library(lemon::dawn ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

# @TODO Release version apparently compiled without d3d12/vulkan support - no adapters can be discovered
set(DAWN_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/Debug)

target_link_libraries(${PROJECT_NAME} PUBLIC ${DAWN_LIB_PATH}/dawn_native.dll.lib)
target_link_libraries(${PROJECT_NAME} PUBLIC ${DAWN_LIB_PATH}/dawn_platform.dll.lib)
target_link_libraries(${PROJECT_NAME} PUBLIC ${DAWN_LIB_PATH}/dawn_proc.dll.lib)

file(GLOB DAWN_BINARIES
        "${DAWN_LIB_PATH}/*.dll"
        "${DAWN_LIB_PATH}/*.dll.exp"
        "${DAWN_LIB_PATH}/*.dll.pdb")

file(COPY ${DAWN_BINARIES} DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
