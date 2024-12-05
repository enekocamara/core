cmake_minimum_required(VERSION 3.14)
project(Scripts CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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


# Get the current date and time
execute_process(
  COMMAND date "+%Y%m%d%H%M%S"
  OUTPUT_VARIABLE CURRENT_DATE_TIME
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(DLL_NAME "Scripts_${CURRENT_DATE_TIME}.dll")


add_library(Scripts SHARED ${SOURCES})

# Set the output name for the DLL
set_target_properties(Scripts PROPERTIES
  OUTPUT_NAME ${DLL_NAME}
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_link_libraries(Scripts PRIVATE Sandbox)
#target_link_libraries(Sandbox PRIVATE Scripts)
target_include_directories(Scripts PRIVATE
    ${CMAKE_SOURCE_DIR}/Syris/vendor/fastNoise
    ${CMAKE_SOURCE_DIR}/Sandbox/src
)

target_include_directories(Scripts PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
if(MSVC)
    add_compile_options(/MP)
else()
    add_compile_options(-j$(nproc))
endif()