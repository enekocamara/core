project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
        "src/**.hpp",
		"src/**.cpp"
	}

	includedirs
    {
        "%{wks.location}/Syris/vendor/entt",
        "%{wks.location}/Syris/vendor/fastNoise",
        "%{wks.location}/Syris/vendor/glm",
        "%{wks.location}/Syris/vendor/stb",
        "%{wks.location}/Syris/src",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
	}

	links
	{
		"Syris"
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

