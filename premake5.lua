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

group "Dependencies"
	include "Puppet/vendor/GLFW"
	include "Puppet/vendor/Glad"
	include "Puppet/vendor/imgui"
group ""

project "Puppet"
	location "Puppet"
	kind "SharedLib"
	language "c++"

	targetdir("bin/".. outputdir .."/%{prj.name}")
	objdir("bin-int/".. outputdir .."/%{prj.name}")

	pchheader "PPpch.h"
	pchsource "Puppet/src/PPpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
	}
	links
	{
		"ImGui",
		"GLFW",
		"Glad",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "off"
		systemversion "latest"
		defines 
		{
			"PP_PLATFORM_WINDOWS",
			"PP_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}
		postbuildcommands
		{
			{"{COPY} %{cfg.buildtarget.relpath} \"../bin/"..outputdir.."/Sandbox/\""}
		}

	filter "configurations:Debug"
		runtime "Debug"
		defines 
		{
			"PP_DEBUG"
		}
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		defines "PP_Release"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		defines "PP_Dist"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "c++"

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
		"Puppet/src"
	}
	links
	{
		"Puppet"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "off"
		systemversion "latest"
		defines 
		{
			"PP_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		runtime "Debug"
		defines "PP_DEBUG"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		defines "PP_Release"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		defines "PP_Dist"
		optimize "On"