project(lemon-scheduler)

set(SOURCE_FILES
        src/lemon/Scheduler.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
add_library(lemon::scheduler ALIAS ${PROJECT_NAME})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        PRIVATE
        ${PROJECT_SOURCE_DIR}/src)

find_package(folly CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Folly::folly)