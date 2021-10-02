project(demo)

add_executable(${PROJECT_NAME} src/main.cpp src/metadata.cpp src/bundles.cpp src/mesh.cpp src/shader.cpp)

target_link_libraries(${PROJECT_NAME} lemon::core)