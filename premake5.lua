workspace "Puppet"
	architecture "x86_64"
	startproject "Puppet-Editor"
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
outputdir ="%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Puppet/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Puppet/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Puppet/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/Puppet/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Puppet/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Puppet/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Puppet/vendor/yaml-cpp/include"

group "Dependencies"
	include "Puppet/vendor/GLFW"
	include "Puppet/vendor/Glad"
	include "Puppet/vendor/imgui"
	include "Puppet/vendor/yaml-cpp"
group ""

include "Puppet"
include "Sandbox"
include "Puppet-Editor"