cmake_minimum_required(VERSION 3.20)

project(lemon-shared)

set(SOURCE_FILES
    "src/lemon/shared/filesystem.cpp"
    "src/lemon/shared/HashStringStore.cpp"
    "src/lemon/shared/utils.cpp"
    "src/lemon/shared/math.cpp"
    "src/lemon/shared/logger.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
add_library(lemon::shared ALIAS ${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
    GLM_FORCE_CTOR_INIT
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    GLM_FORCE_LEFT_HANDED
    PRIVATE
    "$<$<CONFIG:DEBUG>:LEMON_DEBUG>")

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src)

find_package(folly CONFIG REQUIRED)
find_package(tl-expected CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC
    glm::glm
    Folly::folly
    tl::expected
    Microsoft.GSL::GSL
    magic_enum::magic_enum)
