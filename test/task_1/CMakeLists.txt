cmake_minimum_required(VERSION 3.15)
project(task_1)

set(CMAKE_CXX_STANDARD 23)

# „·›«  «·„’œ— «·Œ«’… »«·„Â„…
file(GLOB_RECURSE TASK_SRC "*.cpp")

# Include directories: «” ⁄„· includes „‰ «·„‘—Ê⁄ «·—∆Ì”Ì
include_directories(
    ../src        # √ﬂÊ«œ «·„‘—Ê⁄ «·√’·Ì
    ../lib/glm
    ../lib
)

# ≈‰‘«¡ «·‹ executable ··„Â„…
add_executable(task_1 ${TASK_SRC} "stb_image.cpp")

# —»ÿ «·„ﬂ »«  «·„ÊÃÊœ… „”»ﬁ« ›Ì «·„‘—Ê⁄ «·—∆Ì”Ì
target_link_libraries(task_1
    glfw
    glad
    lodepng
)

# ‰”Œ assets Ê Shaders ≈–« ·“„
add_custom_command(
    TARGET task_1 PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/../test/assets
            ${CMAKE_CURRENT_BINARY_DIR}/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/../test/Shaders
            ${CMAKE_CURRENT_BINARY_DIR}/Shaders
)
