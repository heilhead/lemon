project(lemon-core)

set(SOURCE_FILES
        src/engine.cpp
        src/utils/utils.cpp
        src/device/Window.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
add_library(lemon::core ALIAS ${PROJECT_NAME})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/lib/glfw/include
        ${PROJECT_SOURCE_DIR}/lib/dawn2/include)

set(GLFW_BUILD_DOCS false)
set(GLFW_BUILD_EXAMPLES false)
set(GLFW_BUILD_TESTS false)
set(GLFW_INSTALL false)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/glfw)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/dawn)

target_link_libraries(${PROJECT_NAME} PUBLIC glfw lemon-dawn)
