cmake_minimum_required(VERSION 3.6)

# Set the project name and supported languages
project(game CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the output directory structure
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")

# Enable multi-processor compilation (if supported by the compiler)
if(MSVC)
    add_compile_options(/MP)
else()
    add_compile_options(-j$(nproc))
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PREDEFINED_TARGETS_FOLDER "CustomTargets")

# Add subdirectories for projects
add_subdirectory(Syris)
add_subdirectory(Syris/vendor/imgui)
add_subdirectory(Syris/vendor/Glad)
add_subdirectory(Syris/vendor/GLFW)
add_subdirectory(Syris/vendor/spdlog)
#add_subdirectory(Core)
add_subdirectory(Sandbox)

set_target_properties(glfw PROPERTIES FOLDER "vendor/GLFW3")
set_target_properties(uninstall PROPERTIES FOLDER "vendor/GLFW3")
set_target_properties(update_mappings PROPERTIES FOLDER "vendor/GLFW3")
set_target_properties(ImGui PROPERTIES FOLDER "vendor")
set_target_properties(Glad PROPERTIES FOLDER "vendor")
set_target_properties(spdlog PROPERTIES FOLDER "vendor")

set_property(GLOBAL PROPERTY VS_STARTUP_PROJECT Sandbox)
