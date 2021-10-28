project(test)

include(${LEMON_CPM_CMAKE_PATH})

add_executable(${PROJECT_NAME}
        "src/main.cpp"
        "src/AtomicCache.cpp"
        "src/Memory.cpp"
        "src/Singleton.cpp"
        "src/UnsafeSingleton.cpp"
        "src/Delegate.cpp"
        "src/FreeList.cpp"
        "src/SlotMap.cpp")

target_precompile_headers(${PROJECT_NAME} PRIVATE
    <lemon/pch/folly.h>
    <lemon/pch/std.h>
)

find_package(Catch2 CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Catch2::Catch2
        lemon::core)
