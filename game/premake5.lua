workspace "game"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "engine/vendor/GLFW/include"
IncludeDir["Glad"] = "engine/vendor/Glad/include"
IncludeDir["ImGui"] = "engine/vendor/imgui"

include "engine/vendor/GLFW"
include "engine/vendor/Glad"
include "engine/vendor/ImGui"

project "engine"
    kind "ConsoleApp"
    language "C++"
    targetdir ("bin/" .. outputdir .. "/%{prj.name")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.hpp"
    }

    includedirs
    {
        "%{prj.name}/vendor/entt",
        "%{prj.name}/vendor/fastNoise",
        "%{prj.name}/vendor/glm",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
    }

    links
    {
        "GLFW",
        "opengl32.lib",
        "Glad",
        "ImGui"
    }

    filter "system:windows"
        cppdialect "C++20"
        systemversion "latest"

        defines
        {
            "PLATFORM_WINDOWS",
            "GLFW_INCLUDE_NONE"
        }


    filter "configurations:Debug"
        defines "GAME_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "GAME_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "GAME_Dist"
        optimize "On"

