project(demo)

add_executable(${PROJECT_NAME}
	"src/main.cpp"
	"src/metadata.cpp"
	"src/bundles.cpp"
	"src/mesh.cpp"
    "src/common/DemoModelActor.cpp"
    "src/render/passes/MainRenderPass.cpp"
    "src/render/passes/DebugUIRenderPass.cpp"
    "src/render/passes/PostProcessRenderPass.cpp")

target_compile_definitions(${PROJECT_NAME} PUBLIC LEMON_ENABLE_SLOW_ASSERT)

target_precompile_headers(${PROJECT_NAME} PRIVATE
    <lemon/pch/folly.h>
    <lemon/pch/shared.h>
    <lemon/pch/std.h>
    <lemon/pch/cereal.h>
    <lemon/pch/dawn.h>
    <lemon/pch/imgui.h>
    <lemon/pch/glm.h>
    <lemon/engine.h>
    <lemon/misc.h>
    <lemon/resources.h>
    <lemon/scheduler.h>
    <lemon/serialization.h>
)

target_link_libraries(${PROJECT_NAME} lemon::core)
