cmake_minimum_required(VERSION 3.14)

project(Glad CXX)

add_library(Glad STATIC 
    src/glad.c
)

# Include directories specific to Syris
target_include_directories(Glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
