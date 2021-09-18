project(fbx2lem)

add_executable(${PROJECT_NAME} src/main.cpp src/fbx.cpp)

find_package(assimp CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} lemon::core assimp::assimp)
