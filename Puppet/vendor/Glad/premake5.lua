-- Glad proj premake5.lua
-- 当前所在的文件夹为Glad项目
project "Glad"
    -- 这个项目是一个静态库
	kind "StaticLib"
	-- C语言项目
	language "C"

	-- 生成可执行文件或者库文件的目录，prj是project的别名
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	-- 生成调试文件的目录
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	-- 当前项目需要包含哪些文件
	files
	{
		"include/glad/glad.h",
		"include/KHR/khrplatform.h",
		"src/glad.c"
	}

	-- 当前项目包含的目录
	includedirs
	{
		"include"
	}

	-- 设置平台属性
	filter "system:windows"
		systemversion "latest"
		staticruntime "off"
	
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
        symbols "off"