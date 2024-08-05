IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Syris/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Syris/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Syris/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/Syris/vendor/glm"

workspace "game"
    architecture "x64"
    startproject "Sandbox"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }
    flags
	{
		"MultiProcessorCompile"
	}



outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
group "Syris"
	include "Syris"
group "Core"
    include "Core"

group "Misc"
	include "Sandbox"
group ""
