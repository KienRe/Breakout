workspace "Breakout"
	architecture "x64"
	configurations
	{
		"Debug",
		"Release",
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Breakout"
	location "Breakout"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Breakout/src/**.h",
		"Breakout/src/**.cpp"
	}

	includedirs
	{
		"Breakout/src",
		"vendor/sdl/include",
		"vendor/glm",
		"vendor/glew/include"
	}
	
	libdirs
	{
		"vendor/sdl/lib/x64",
		"vendor/glew/lib/Release/x64"
	}
	
	links
	{
		"SDL2.lib",
		"SDL2main.lib",
		"glew32.lib",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "10.0.17134.0"

		defines
		{
			"PLATFORM_WINDOWS"
		}
		
		postbuildcommands
		{
			("{COPY} %{wks.location}vendor/sdl/lib/x64/SDL2.dll %{wks.location}bin/%{outputdir}/Breakout"),
			("{COPY} %{wks.location}vendor/glew/bin/Release/x64/glew32.dll %{wks.location}bin/%{outputdir}/Breakout")
		}

	filter "configurations:Debug"
		defines "DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "RELEASE"
		optimize "On"