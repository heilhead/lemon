project(test)

include(${LEMON_CPM_CMAKE_PATH})

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/engine_tests.cpp)

find_package(Catch2 CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME}
        Catch2
        lemon::core
        lemon::shared)
