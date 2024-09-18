#cmake_minimum_required(VERSION 3.30.2)
cmake_minimum_required(VERSION 3.19)
# Set the project name and supported languages
project(game CXX)

#set(HPX_DIR "${CMAKE_BINARY_DIR}/install")

#include(FetchContent)

# Set the C++ standard
#set(CMAKE_CXX_STANDARD 20)
#set(HPX_WITH_CXX_STANDARD=20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation directory")

# Set the output directory structure
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PREDEFINED_TARGETS_FOLDER "CustomTargets")

# Enable multi-processor compilation (if supported by the compiler)
if(MSVC)
    add_compile_options(/MP)
else()
    add_compile_options(-j$(nproc))
endif()



#set(BOOST_ROOT "/Users/eneko/dev/boost/boost_1_82_0")
#set(BOOST_LIBRARYDIR "/Users/eneko/dev/boost/boost_1_82_0/stage/lib")


#set(Asio_ROOT "${CMAKE_SOURCE_DIR}/Syris/vendor/asio/asio")

# Add subdirectories for projects


#set(__HOLD ${CMAKE_INSTALL_INCLUDEDIR})
#set(CMAKE_INSTALL_INCLUDEDIR "${CMAKE_BINARY_DIR}/Syris/vendor/hwloc/contrib/windows-cmake/include")
#add_subdirectory(Syris/vendor/hwloc/contrib/windows-cmake)
#set(CMAKE_INSTALL_INCLUDEDIR ${__HOLD})

#find_package(hwloc REQUIRED)

#find_package(hwloc REQUIRED HINTS ${CMAKE_SOURCE_DIR}/Syris/vendor/hwloc/contrib/windows-cmake/)

#set(Hwloc_INCLUDE_DIR "${CMAKE_BINARY_DIR}/Syris/vendor/hwloc/contrib/windows-cmake/build/include")

#set(Hwloc_ROOT "${CMAKE_BINARY_DIR}/Syris/vendor/hwloc/")
#set(HWLOC_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/build/Syris/vendor/hwloc/include")
#set(Hwloc_LIBRARY "${CMAKE_BINARY_DIR}/Syris/vendor/hwloc/lib/hwloc.lib")



#find_package(HPX REQUIRED)

#if (NOT TARGET HWLOC::hwloc)
#    find_path(HWLOC_INCLUDE_DIR hwloc.h
#        HINTS ${CMAKE_PREFIX_PATH}/include
#    )

#    find_library(HWLOC_LIBRARY NAMES hwloc
#        HINTS ${CMAKE_PREFIX_PATH}/lib
#    )

#   add_library(HWLOC::hwloc UNKNOWN IMPORTED)
#    set_target_properties(HWLOC::hwloc PROPERTIES
#        IMPORTED_LOCATION ${HWLOC_LIBRARY}
#        INTERFACE_INCLUDE_DIRECTORIES ${HWLOC_INCLUDE_DIR}
#    )
#endif()

#set(HPX_BUILD_EXAMPLES OFF CACHE BOOL "Build HPX examples")
#set(HPX_BUILD_TESTS OFF CACHE BOOL "Build HPX tests")
#set(HPX_WITH_EXAMPLES OFF CACHE BOOL "Enable HPX examples")
#set(HPX_WITH_TESTS OFF CACHE BOOL "Enable HPX tests")

#set(HPX_DIR "${CMAKE_BINARY_DIR}/Syris/vendor/hpx/lib/cmake/HPX")
#add_subdirectory(Syris/vendor/hpx)

#add_subdirectory(Syris/vendor/asio)
#set(Hwloc_ROOT "${CMAKE_SOURCE_DIR}/Syris/vendor/hwloc/contrib/windows-cmake")
#FetchContent_Declare(
#  hwloc
#  GIT_REPOSITORY https://github.com/open-mpi/hwloc.git
#  GIT_TAG master  # Use the desired version
#  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/hwloc-install
#)
#FetchContent_MakeAvailable(hwloc)

add_subdirectory(Sandbox)
add_subdirectory(Core)

add_subdirectory(Syris)
add_subdirectory(Syris/vendor/imgui)
add_subdirectory(Syris/vendor/imguiDocking)
add_subdirectory(Syris/vendor/Glad)
add_subdirectory(Syris/vendor/GLFW)
add_subdirectory(Syris/vendor/spdlog)



#add_subdirectory(Syris/vendor/googletests)


set_target_properties(glfw PROPERTIES FOLDER "vendor/GLFW3")
set_target_properties(uninstall PROPERTIES FOLDER "vendor/GLFW3")
set_target_properties(update_mappings PROPERTIES FOLDER "vendor/GLFW3")
set_target_properties(ImGui PROPERTIES FOLDER "vendor")
set_target_properties(ImGuiDocking PROPERTIES FOLDER "vendor")
set_target_properties(Glad PROPERTIES FOLDER "vendor")
set_target_properties(spdlog PROPERTIES FOLDER "vendor")
#set_target_properties(HPX PROPERTIES FOLDER "vendor")
#set_target_properties(googletest-distribution PROPERTIES FOLDER "vendor")
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

            # file(GLOB_RECURSE cmake_files ${CMAKE_SOURCE_DIR}/${base_dir}/${subdir}/*.cmake)

            #foreach(cmake_file IN LISTS cmake_files)
            #  include(${cmake_file})
            #endforeach()

            # Apply folder property to all targets
            # Note: This step requires that targets be defined in the included CMakeLists.txt files
            #foreach(target ${_targets})
            #if (TARGET ${target})
            #    set_target_properties(${target} PROPERTIES FOLDER ${folder_name})
            #endif()
            #endforeach()
        endif()
    endforeach()
endfunction()

# Add subdirectories

# Apply folder properties
#set_target_properties_for_subdirectory("Syris/vendor/hwloc/contrib/windows-cmake" "vendor/Hwloc")
