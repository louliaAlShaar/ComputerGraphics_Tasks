cmake_minimum_required(VERSION 3.15)
project(test)

set(CMAKE_CXX_STANDARD 23)

# Assimp
add_library(assimp STATIC IMPORTED)
set_target_properties(assimp PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/assimp/lib/assimp-vc143-mt.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/lib/assimp/include"
)





# Add source files
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

# Includes
include_directories(include)
include_directories(lib/glm)
include_directories(lib)           # stb

# Executable
add_executable(main ${SOURCE_FILES})


# GLFW
add_subdirectory(lib/glfw)

# GLAD
add_library(glad STATIC lib/glad/src/glad.c)
target_include_directories(glad PUBLIC lib/glad/include)

# lodepng
add_library(lodepng STATIC lib/lodepng/lodepng.cpp)
target_include_directories(lodepng PUBLIC lib/lodepng)

# Link libraries
target_link_libraries(main
    glfw
    glad
    lodepng
    assimp        
)

# task_1
add_subdirectory(task_1)

# Copy assets & shaders
add_custom_command(
    TARGET main PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Shaders ${CMAKE_CURRENT_BINARY_DIR}/Shaders
)
