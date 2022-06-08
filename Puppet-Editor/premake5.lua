project "Puppet-Editor"
	language "C++"
	cppdialect "C++17"
	kind "ConsoleApp"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	files
	{
		"src/**.h",
		"src/**.cpp"
	}
	includedirs
	{
		"%{wks.location}/Puppet/vendor/spdlog/include",
		"%{wks.location}/Puppet/src",
        "%{wks.location}/Puppet/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
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