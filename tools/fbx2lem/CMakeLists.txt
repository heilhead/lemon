project(fbx2lem)

add_executable(${PROJECT_NAME} "src/main.cpp" "src/fbx.cpp")

find_package(assimp CONFIG REQUIRED)

target_precompile_headers(${PROJECT_NAME} PRIVATE
    <lemon/pch/folly.h>
    <lemon/pch/shared.h>
    <lemon/pch/std.h>
    <lemon/pch/cereal.h>
    <lemon/pch/dawn.h>
    <lemon/pch/glm.h>
)

target_link_libraries(${PROJECT_NAME} lemon::core assimp::assimp)
