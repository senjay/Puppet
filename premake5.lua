workspace "Puppet"
	architecture "x64"
	startproject "Sandbox"
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
outputdir ="%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
IncludeDir = {}
IncludeDir["GLFW"] = "Puppet/vendor/GLFW/include"
IncludeDir["Glad"] = "Puppet/vendor/Glad/include"
IncludeDir["ImGui"] = "Puppet/vendor/imgui"
IncludeDir["glm"] = "Puppet/vendor/glm"

group "Dependencies"
	include "Puppet/vendor/GLFW"
	include "Puppet/vendor/Glad"
	include "Puppet/vendor/imgui"
group ""

project "Puppet"
	location "Puppet"
	language "C++"
	cppdialect "C++17"
	kind "StaticLib"
	staticruntime "off"

	targetdir("bin/".. outputdir .."/%{prj.name}")
	objdir("bin-int/".. outputdir .."/%{prj.name}")

	pchheader "PPpch.h"
	pchsource "Puppet/src/PPpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
	}
	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
	}
	links
	{
		"ImGui",
		"GLFW",
		"Glad",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"
		defines 
		{
			"PP_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE",
		}

	filter "configurations:Debug"
		runtime "Debug"
		defines 
		{
			"PP_DEBUG"
		}
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		defines "PP_Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		defines "PP_Dist"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	language "C++"
	cppdialect "C++17"
	kind "ConsoleApp"
	staticruntime "off"

	targetdir("bin/".. outputdir .."/%{prj.name}")
	objdir("bin-int/".. outputdir .."/%{prj.name}")
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	includedirs
	{
		"Puppet/vendor/spdlog/include",
		"Puppet/src",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
	}
	links
	{
		"Puppet"
	}

	filter "system:windows"
		systemversion "latest"
		defines 
		{
			"PP_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		runtime "Debug"
		defines "PP_DEBUG"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		defines "PP_Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		defines "PP_Dist"
		optimize "on"