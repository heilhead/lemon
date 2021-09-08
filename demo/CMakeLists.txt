project(demo)

add_executable(${PROJECT_NAME} src/main.cpp)

find_package(folly CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Folly::folly)

#target_link_libraries(${PROJECT_NAME}
#        lemon::core
#        lemon::shared
#        lemon::scheduler)
