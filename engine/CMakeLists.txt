project(lemon-core)

set(SOURCE_FILES
        src/lemon/Triangle.cpp
        src/lemon/engine.cpp
        src/lemon/device/Window.cpp
        src/lemon/resource/ResourceInstance.cpp
        src/lemon/resource/ResourceManager.cpp
        src/lemon/resource/ResourceStore.cpp
        src/lemon/resource/ResourceContract.cpp
        src/lemon/resource/ResourceLocation.cpp
        src/lemon/resource/types/MaterialResource.cpp
        src/lemon/resource/types/TextureResource.cpp
        src/lemon/resource/types/BundleResource.cpp
        src/lemon/resource/common.cpp
        src/lemon/shared/Vector.cpp
        src/lemon/shared/filesystem.cpp
        src/lemon/utils/utils.cpp
        src/lemon/scheduler/Scheduler.cpp
        src/lemon/scheduler/utils.cpp
        )

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
find_package(folly CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
find_package(tl-expected CONFIG REQUIRED)
find_package(cereal CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)

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

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        lemon-dawn
        glm::glm
        Folly::folly
        tl::expected
        cereal
        magic_enum::magic_enum
        ozz_animation
        PRIVATE
        range-v3
        glfw)