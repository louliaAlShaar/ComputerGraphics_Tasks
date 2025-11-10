
# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.12)
project(glad)
# Define the glad library target
add_library(glad
    src/glad.c
    include/glad/glad.h
    include/KHR/khrplatform.h
)

# Specify include directories
target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)