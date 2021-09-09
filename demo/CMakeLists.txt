project(demo)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME}
        lemon::core
        lemon::shared
        lemon::scheduler)
