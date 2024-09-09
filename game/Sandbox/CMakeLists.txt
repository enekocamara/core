cmake_minimum_required(VERSION 3.14)


project(Sandbox CXX)

#find_package(HPX REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the output directory structure
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

# Automatically organize files in Visual Studio
foreach(file ${SOURCES})
    # Get relative path and convert to Windows style
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" relative_path ${file})
    string(REPLACE "/" "\\" relative_path ${relative_path})
    get_filename_component(directory ${relative_path} DIRECTORY)
    
    # Create a source group for each directory
    source_group("${directory}" FILES ${file})
endforeach()

# Include directories specific to Syris
#target_include_directories(Sandbox PRIVATE
#    ${CMAKE_SOURCE_DIR}/Syris/vendor/GLFW/include
#    ${CMAKE_SOURCE_DIR}/Syris/vendor/Glad/include
#    ${CMAKE_SOURCE_DIR}/Syris/vendor/imgui
#    ${CMAKE_SOURCE_DIR}/Syris/vendor/glm
#    ${CMAKE_SOURCE_DIR}/Syris/vendor/spdlog/include
#    ${CMAKE_SOURCE_DIR}/Syris/vendor/entt
#    ${CMAKE_SOURCE_DIR}/Syris/vendor/stb
#    ${CMAKE_SOURCE_DIR}/Syris/vendor/fastNoise
#    ${CMAKE_SOURCE_DIR}/Syris/src
#    ${HPX_INCLUDE_DIRS}
    # other include directories
#)

#include(${CMAKE_SOURCE_DIR}/Syris/vendor/hpx/cmake/installed_hpx.cmake)
#find_package(HPX REQUIRED NO_CMAKE_PACKAGE_REGISTRY)
#if(NOT HPX_FOUND)
#    phylanx_error("HPX could not be found, please set HPX_DIR to help locating it.")
#endif()


set(HPX_LOGLEVEL 0)
add_executable(Sandbox ${SOURCES})
target_link_libraries(Sandbox PRIVATE Core)
target_include_directories(Sandbox PRIVATE
    ${CMAKE_SOURCE_DIR}/Syris/vendor/fastNoise
)
set(HPX_WITH_LOGGING OFF CACHE BOOL "Disable HPX logging" FORCE)
# Enable multi-processor compilation (if supported by the compiler)
if(MSVC)
    add_compile_options(/MP)
else()
    add_compile_options(-j$(nproc))
endif()
