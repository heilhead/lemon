project(lemon-shared)

set(SOURCE_FILES
        src/lemon/shared/Vector.cpp
        src/lemon/shared/filesystem.cpp
        )

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
add_library(lemon::shared ALIAS ${PROJECT_NAME})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        PRIVATE
        ${PROJECT_SOURCE_DIR}/src)

find_package(tl-expected CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC tl::expected)