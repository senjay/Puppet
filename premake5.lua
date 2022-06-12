VULKAN_SDK = os.getenv("VULKAN_SDK")

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
IncludeDir["ImGuizmo"] = "%{wks.location}/Puppet/vendor/ImGuizmo"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"


LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{wks.location}/Puppet/vendor/VulkanSDK/Lib"
LibraryDir["VulkanSDK_DebugDLL"] = "%{wks.location}/Puppet/vendor/VulkanSDK/Bin"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"


group "Dependencies"
	include "Puppet/vendor/GLFW"
	include "Puppet/vendor/Glad"
	include "Puppet/vendor/imgui"
	include "Puppet/vendor/yaml-cpp"
group ""

include "Puppet"
include "Sandbox"
include "Puppet-Editor"