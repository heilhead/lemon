project(demo)

add_executable(${PROJECT_NAME} src/main.cpp src/metadata.cpp src/bundles.cpp)

find_package(cereal CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME}
        cereal
        lemon::core)
