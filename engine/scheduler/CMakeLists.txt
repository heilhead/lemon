project(lemon-scheduler)

set(SOURCE_FILES
        src/lemon/scheduler/Scheduler.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
add_library(lemon::scheduler ALIAS ${PROJECT_NAME})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        PRIVATE
        ${PROJECT_SOURCE_DIR}/src)

find_package(folly CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
find_package(tl-expected CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC Folly::folly range-v3 tl::expected)
