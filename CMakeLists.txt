cmake_minimum_required(VERSION 3.28 FATAL_ERROR)
set(PROJECT_NAME Snake)
project(${PROJECT_NAME})

add_executable(${PROJECT_NAME} src/snake.cpp)

add_subdirectory(external/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

add_subdirectory(external/glad)
target_link_libraries(${PROJECT_NAME} glad)

add_subdirectory(external/glm)
target_link_libraries(${PROJECT_NAME} glm)