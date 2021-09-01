project(lemon-utils)

add_library(${PROJECT_NAME} INTERFACE)
add_library(lemon::utils ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
        INTERFACE
        ${PROJECT_SOURCE_DIR}/include
        )
