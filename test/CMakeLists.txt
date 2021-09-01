project(test)

include_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/engine_tests.cpp
        )

CPMAddPackage("gh:catchorg/Catch2@2.5.0")

target_link_libraries(${PROJECT_NAME}
        Catch2
        lemon::core
        lemon::utils
        )
