cmake_minimum_required(VERSION 3.14)

project(EntryPoint CXX)

set(CMAKE_CXX_STANDARD 23)
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

add_executable(EntryPoint ${SOURCES})
target_link_libraries(EntryPoint PUBLIC %PROJECT_NAME% Syris)
target_include_directories(EntryPoint PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(MSVC)
    add_compile_options(/MP)
else()
    add_compile_options(-j$(nproc))
endif()