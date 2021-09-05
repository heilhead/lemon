project(lemon-core)

set(SOURCE_FILES
        src/lemon/engine.cpp
        src/lemon/utils/utils.cpp
        src/lemon/device/Window.cpp
        src/lemon/Triangle.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
add_library(lemon::core ALIAS ${PROJECT_NAME})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        PRIVATE
        ${PROJECT_SOURCE_DIR}/src)

include(${LEMON_CPM_CMAKE_PATH})

CPMAddPackage(
        NAME nlohmann_json
        GITHUB_REPOSITORY "nlohmann/json"
        VERSION 3.10.2
        GIT_SHALLOW true
        OPTIONS "JSON_BuildTests OFF")

CPMAddPackage(
        NAME glm
        GITHUB_REPOSITORY "g-truc/glm"
        GIT_TAG "6ad79aa"
        GIT_SHALLOW true)

CPMAddPackage(
        NAME glfw
        GITHUB_REPOSITORY "glfw/glfw"
        GIT_TAG "3.3.4"
        GIT_SHALLOW true
        OPTIONS
        "GLFW_BUILD_DOCS OFF"
        "GLFW_BUILD_EXAMPLES OFF"
        "GLFW_BUILD_TESTS OFF"
        "GLFW_INSTALL OFF")

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/dawn)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        lemon-dawn
        glfw
        glm::glm
        nlohmann_json::nlohmann_json)
