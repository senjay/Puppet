project "Puppet"
	language "C++"
	cppdialect "C++17"
	kind "StaticLib"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "PPpch.h"
	pchsource "src/PPpch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",
		"vendor/stb_image/**.h",
		"vendor/stb_image/**.cpp",
	}
	includedirs
	{
		"src",
		"vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
	}
	links
	{
		"ImGui",
		"GLFW",
		"Glad",
		"yaml-cpp",
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