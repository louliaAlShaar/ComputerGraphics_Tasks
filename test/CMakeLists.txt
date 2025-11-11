cmake_minimum_required(VERSION 3.15)
project(test)

set(CMAKE_CXX_STANDARD 23)

# Add source files

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")


# Add include directories

include_directories(include)

# Add executable

add_executable(main ${SOURCE_FILES})

# Add GLFW

add_subdirectory(lib/glfw)

# Add GLAD

add_library(glad STATIC lib/glad/src/glad.c )
target_include_directories(glad PUBLIC lib/glad/include)

# Add lodepng
add_library(lodepng STATIC lib/lodepng/lodepng.cpp )
target_include_directories(lodepng PUBLIC lib/lodepng)

# GLM (header-only)

include_directories(lib/glm)

# STB (header-only)
include_directories(lib)

# Link libraries

target_link_libraries(main
glfw
glad
lodepng
)

add_subdirectory(task_1)


# Copy assets and Shaders folder instead of symlink (Windows-friendly)

add_custom_command(
TARGET main PRE_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_SOURCE_DIR}/Shaders ${CMAKE_CURRENT_BINARY_DIR}/Shaders
)

