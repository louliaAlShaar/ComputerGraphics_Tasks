cmake_minimum_required(VERSION 3.15)
project(task_1)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE TASK_SRC "*.cpp")

include_directories(
    ../src       
    ../lib/glm
    ../lib
)

add_executable(task_1 ${TASK_SRC} "stb_image.cpp")

target_link_libraries(task_1
    glfw
    glad
    lodepng
)

add_custom_command(
    TARGET task_1 PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/../test/assets
            ${CMAKE_CURRENT_BINARY_DIR}/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/../test/Shaders
            ${CMAKE_CURRENT_BINARY_DIR}/Shaders
)
