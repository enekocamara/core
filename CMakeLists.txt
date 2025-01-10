#cmake_minimum_required(VERSION 3.30.2)
cmake_minimum_required(VERSION 3.19)
# Set the project name and supported languages
project(game CXX)


set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PREDEFINED_TARGETS_FOLDER "CustomTargets")



# Enable multi-processor compilation (if supported by the compiler)
if(MSVC)
    add_compile_options(/MP)
else()
    add_compile_options(-j$(nproc))
endif()


set(BUILD_GMOCK OFF CACHE BOOL "Disable Google Mock" FORCE)

add_subdirectory(Sandbox)
add_subdirectory(Core)
add_subdirectory(GameEngine)
add_subdirectory(Sandbox/scripts)
#add_subdirectory(Test)
add_subdirectory(Syris/tests)

add_subdirectory(Syris)
add_subdirectory(Syris/tests/resources/dll)
add_subdirectory(Syris/vendor/imgui)
add_subdirectory(Syris/vendor/imguiDocking)
add_subdirectory(Syris/vendor/Glad)
add_subdirectory(Syris/vendor/GLFW)
add_subdirectory(Syris/vendor/spdlog)
add_subdirectory(Syris/vendor/googletests)

#add_subdirectory(Syris/vendor/googletests)

set_target_properties(glfw PROPERTIES FOLDER "vendor/GLFW3")
set_target_properties(uninstall PROPERTIES FOLDER "vendor/GLFW3")
set_target_properties(update_mappings PROPERTIES FOLDER "vendor/GLFW3")
set_target_properties(ImGui PROPERTIES FOLDER "vendor")
set_target_properties(ImGuiDocking PROPERTIES FOLDER "vendor")
set_target_properties(Glad PROPERTIES FOLDER "vendor")
set_target_properties(spdlog PROPERTIES FOLDER "vendor")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Sandbox)


function(set_target_properties_for_subdirectory subdir folder_name)
  get_property(targets DIRECTORY ${CMAKE_SOURCE_DIR}/${subdir} PROPERTY BUILDSYSTEM_TARGETS)
  
  foreach(target IN LISTS targets)
    if (TARGET ${target})
      set_target_properties(${target} PROPERTIES FOLDER ${folder_name})
    endif()
  endforeach()
endfunction()

function(set_target_properties_recursively base_dir folder_name)
    # Get all subdirectories
    file(GLOB_RECURSE subdirs RELATIVE ${CMAKE_SOURCE_DIR}/${base_dir} ${CMAKE_SOURCE_DIR}/${base_dir}/*)

    foreach(subdir IN LISTS subdirs)
        if (IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${base_dir}/${subdir})
            # Iterate through CMakeLists.txt in the subdirectory
            get_property(targets DIRECTORY ${CMAKE_SOURCE_DIR}/${base_dir}/${subdir} PROPERTY BUILDSYSTEM_TARGETS)

            foreach(target IN LISTS targets)
                if (TARGET ${target})
                    set_target_properties(${target} PROPERTIES FOLDER ${folder_name})
                endif()
            endforeach()

        endif()
    endforeach()
endfunction()
