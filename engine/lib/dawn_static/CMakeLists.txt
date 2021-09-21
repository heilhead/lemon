project(lemon-dawn-static)

set(SOURCE_FILES src/dawn/webgpu_cpp.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
add_library(lemon::dawn_static ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw dxguid)

set(DAWN_STATIC_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>)

target_link_libraries(${PROJECT_NAME} PUBLIC ${DAWN_STATIC_LIB_PATH}/dawn_native_static.lib)
target_link_libraries(${PROJECT_NAME} PUBLIC ${DAWN_STATIC_LIB_PATH}/dawn_platform_static.lib)
target_link_libraries(${PROJECT_NAME} PUBLIC ${DAWN_STATIC_LIB_PATH}/dawn_proc_static.lib)
