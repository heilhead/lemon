project(lemon-imgui)

set(SOURCE_FILES 
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/misc/cpp/imgui_stdlib.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_wgpu.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
add_library(lemon::imgui ALIAS ${PROJECT_NAME})

find_package(glfw3 CONFIG REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR} PRIVATE ${PROJECT_SOURCE_DIR}/imgui)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw lemon::dawn_static)
