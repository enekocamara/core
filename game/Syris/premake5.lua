include "vendor/GLFW"
include "vendor/Glad"
include "vendor/imGui"

project "Syris"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin/-int" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.hpp",
        "src/**.cpp"
    }
    includedirs
    {
        "vendor/entt",
        "vendor/fastNoise",
        "vendor/glm",
        "vendor/stb",
        "vendor/spdlog/include",
        "src",
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
            "GLFW_INCLUDE_NONE",
            "RENDER_API_OPENGL"
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

