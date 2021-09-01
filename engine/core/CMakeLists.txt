project(lemon-core)

add_library(${PROJECT_NAME} src/engine.cpp)
add_library(lemon::core ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
        PUBLIC ${PROJECT_SOURCE_DIR}/include
        )
