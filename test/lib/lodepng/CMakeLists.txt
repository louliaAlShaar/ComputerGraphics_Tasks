cmake_minimum_required(VERSION 3.21)
project(lodepng)

add_library(lodepng lodepng.cpp)

# Specify include directories for LodePNG
target_include_directories(lodepng
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set C++ standard
target_compile_features(lodepng PUBLIC cxx_std_11)