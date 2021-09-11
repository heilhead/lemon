project(lemon-core)

set(SOURCE_FILES
        src/lemon/engine.cpp
        src/lemon/utils/utils.cpp
        src/lemon/device/Window.cpp
        src/lemon/resource/Resource.cpp
        src/lemon/Triangle.cpp
        src/lemon/resource/ResourceManager.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
add_library(lemon::core ALIAS ${PROJECT_NAME})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

#target_compile_options(${PROJECT_NAME} PRIVATE
#        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
#        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        PRIVATE
        ${PROJECT_SOURCE_DIR}/src)

include(${LEMON_CPM_CMAKE_PATH})

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/dawn)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        lemon-shared
        lemon-scheduler
        glm::glm
        PRIVATE
        lemon-dawn
        glfw)