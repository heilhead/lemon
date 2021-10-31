project(demo)

add_executable(${PROJECT_NAME}
	"src/main.cpp"
	"src/metadata.cpp"
	"src/bundles.cpp"
	"src/mesh.cpp"
    "src/game/math.cpp"
    "src/game/CameraController.cpp"
    "src/game/actor/Actor.cpp"
    "src/game/actor/GameWorld.cpp"
    "src/game/actor/GameObject.cpp"
    "src/game/actor/GameObjectStore.cpp"
    "src/game/actor/ActorComponent.cpp"
    "src/game/actor/common.cpp"
    "src/game/actor/TickGroup.cpp")

target_precompile_headers(${PROJECT_NAME} PRIVATE
    <lemon/pch/folly.h>
    <lemon/pch/shared.h>
    <lemon/pch/std.h>
    <lemon/pch/cereal.h>
    <lemon/pch/dawn.h>
    <lemon/pch/glm.h>
    <lemon/engine.h>
    <lemon/game.h>
    <lemon/misc.h>
    <lemon/render.h>
    <lemon/resources.h>
    <lemon/scheduler.h>
    <lemon/serialization.h>
)

target_link_libraries(${PROJECT_NAME} lemon::core)
