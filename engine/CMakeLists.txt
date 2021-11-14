project(lemon-core)

set(SOURCE_FILES
        "src/lemon/engine.cpp"
        "src/lemon/device/Window.cpp"
        "src/lemon/resource/ResourceInstance.cpp"
        "src/lemon/resource/ResourceManager.cpp"
        "src/lemon/resource/ResourceStore.cpp"
        "src/lemon/resource/ResourceContract.cpp"
        "src/lemon/resource/ResourceLocation.cpp"
        "src/lemon/resource/types/MaterialResource.cpp"
        "src/lemon/resource/types/TextureResource.cpp"
        "src/lemon/resource/types/BundleResource.cpp"
        "src/lemon/resource/types/ModelResource.cpp"
        "src/lemon/resource/types/model/LemonModel.cpp"
        "src/lemon/resource/common.cpp"
        "src/lemon/scheduler/Scheduler.cpp"
        "src/lemon/scheduler/utils.cpp"
        "src/lemon/render/utils.cpp"
        "src/lemon/render/material/ShaderCompiler.cpp"
        "src/lemon/render/material/ShaderProgram.cpp"
        "src/lemon/device/Platform.cpp"
        "src/lemon/render/ConstantBuffer.cpp"
        "src/lemon/device/GPUDevice.cpp"
        "src/lemon/device/Device.cpp"
        "src/lemon/resource/types/material/MaterialComposer.cpp"
        "src/lemon/render/RenderManager.cpp"
        "src/lemon/render/material/MaterialManager.cpp"
        "src/lemon/render/material/MaterialInstance.cpp"
        "src/lemon/render/material/SurfaceMaterialInstance.cpp"
        "src/lemon/resource/types/texture/PNGDecoder.cpp"
        "src/lemon/render/material/MaterialUniformData.cpp"
        "src/lemon/tasks/filesystem.cpp"
        "src/lemon/render/material/MaterialLayout.cpp"
        "src/lemon/render/PipelineManager.cpp"
        "src/lemon/render/MeshVertexFormat.cpp"
        "src/lemon/render/MeshGPUBuffer.cpp"
        "src/lemon/render/RenderQueue.cpp"
        "src/lemon/render/DebugUI.cpp"
        "src/lemon/device/Input.cpp"
        "src/lemon/device/input/KeyboardListener.cpp"
        "src/lemon/device/input/MouseListener.cpp"
        "src/lemon/game/Camera.cpp"
        "src/lemon/game/Transform.cpp"
        "src/lemon/game/actor/Actor.cpp"
        "src/lemon/game/actor/GameWorld.cpp"
        "src/lemon/game/actor/GameObject.cpp"
        "src/lemon/game/actor/GameObjectStore.cpp"
        "src/lemon/game/actor/ActorComponent.cpp"
        "src/lemon/game/actor/common.cpp"
        "src/lemon/game/actor/TickGroup.cpp"
        "src/lemon/game/actor/components/CameraComponent.cpp"
        "src/lemon/game/actor/components/MovementComponent.cpp"
        "src/lemon/game/actor/actors/FlyingCameraActor.cpp"
        "src/lemon/game/actor/components/MeshComponent.cpp"
        "src/lemon/game/state/GameState.cpp"
        "src/lemon/game/state/GameStateManager.cpp"
        "src/lemon/game/state/InputEvent.cpp"
        "src/lemon/platform/windows.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
add_library(lemon::core ALIAS ${PROJECT_NAME})

#target_compile_options(${PROJECT_NAME} PRIVATE
#        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
#        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        PRIVATE
        ${PROJECT_SOURCE_DIR}/src)

include(${LEMON_CPM_CMAKE_PATH})

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/dawn_static)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/imgui)

find_package(glfw3 CONFIG REQUIRED)
find_package(folly CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
find_package(tl-expected CONFIG REQUIRED)
find_package(cereal CONFIG REQUIRED)
find_package(lodepng CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
    INJA_NOEXCEPTION
    GLFW_EXPOSE_NATIVE_WIN32)

target_compile_definitions(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:LEMON_DEBUG>")

target_precompile_headers(${PROJECT_NAME} PRIVATE
    <lemon/pch/folly.h>
    <lemon/pch/shared.h>
    <lemon/pch/std.h>
    <lemon/pch/cereal.h>
    <lemon/pch/dawn.h>
    <lemon/pch/imgui.h>
    <lemon/pch/glm.h>
)

CPMAddPackage(
	GITHUB_REPOSITORY "guillaumeblanc/ozz-animation"
	GIT_TAG 0.13.0
	OPTIONS
	"ozz_build_tools OFF"
	"ozz_build_fbx OFF"
	"ozz_build_gltf OFF"
	"ozz_build_samples OFF"
	"ozz_build_howtos OFF"
	"ozz_build_tests OFF"
	"ozz_build_msvc_rt_dll OFF")

CPMAddPackage("gh:pantor/inja#635e1fb")

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        lemon::dawn_static
        lemon::imgui
        lemon::shader
        lemon::shared
        Folly::folly
        tl::expected
        cereal
        ozz_animation
        lodepng
        pantor::inja
        yaml-cpp
        PRIVATE
        range-v3
        glfw)