project(lemon-core)

add_library(${PROJECT_NAME} src/engine.cpp)
add_library(lemon::core ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
        PUBLIC ${PROJECT_SOURCE_DIR}/include
        PRIVATE ${PROJECT_SOURCE_DIR}/lib/dawn/include)

set(DAWN_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/dawn/bin/${CMAKE_BUILD_TYPE})

target_link_libraries(${PROJECT_NAME} PUBLIC ${DAWN_LIB_PATH}/dawn_native.lib)
target_link_libraries(${PROJECT_NAME} PUBLIC ${DAWN_LIB_PATH}/dawn_platform.lib)
target_link_libraries(${PROJECT_NAME} PUBLIC ${DAWN_LIB_PATH}/dawn_proc.lib)

file(COPY
        ${DAWN_LIB_PATH}/dawn_native.dll
        ${DAWN_LIB_PATH}/dawn_platform.dll
        ${DAWN_LIB_PATH}/dawn_proc.dll
        DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
