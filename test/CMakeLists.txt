project(test)

include(${LEMON_CPM_CMAKE_PATH})

CPMAddPackage(
        NAME Catch2
        GITHUB_REPOSITORY "catchorg/Catch2"
        VERSION 2.13.7
        GIT_SHALLOW true)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/engine_tests.cpp)

target_link_libraries(${PROJECT_NAME}
        Catch2
        lemon::core
        lemon::utils)
