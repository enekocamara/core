cmake_minimum_required(VERSION 3.14)

project(Syris CXX)

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

add_library(Syris STATIC ${SOURCES})

# Include directories specific to Syris
target_include_directories(Syris PUBLIC
    ${CMAKE_SOURCE_DIR}/Syris/vendor/GLFW/include
    ${CMAKE_SOURCE_DIR}/Syris/vendor/Glad/include
    #${CMAKE_SOURCE_DIR}/Syris/vendor/imgui
    ${CMAKE_SOURCE_DIR}/Syris/vendor/imguiDocking
    ${CMAKE_SOURCE_DIR}/Syris/vendor/glm
    ${CMAKE_SOURCE_DIR}/Syris/vendor/spdlog/include
    ${CMAKE_SOURCE_DIR}/Syris/vendor/entt
    ${CMAKE_SOURCE_DIR}/Syris/vendor/stb
    ${CMAKE_SOURCE_DIR}/Syris/src
    # other include directories
)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

target_link_libraries(Syris PUBLIC ImGuiDocking Glad glfw spdlog ${Boost_LIBRARIES})

target_compile_definitions(Syris PRIVATE
    PLATFORM_WINDOWS
    GLFW_INCLUDE_NONE
    RENDER_API_OPENGL
)

# Enable multi-processor compilation (if supported by the compiler)
if(MSVC)
    add_compile_options(/MP)
else()
    add_compile_options(-j$(nproc))
endif()
