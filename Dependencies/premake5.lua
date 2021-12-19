include("conanbuildinfo.premake.lua")

workspace "Dependencies"
	conan_basic_setup()
	
	configurations { "Debug", "Release" }
	
	project "FlamingDependencies"
		kind "StaticLib"
		language "C++"
		
		targetdir("../Binaries/FlamingDependencies/%{cfg.buildcfg}/")
		objdir("../Temp/FlamingDependencies/%{cfg.buildcfg}/")
		
		includedirs {
			"Headers/",
			"Headers/lua"
		}
		
		files {
			"Source/base64/**.c",
			"Source/base64/**.cpp",
			"Source/lua/*.c",
			"Source/luabind/*.cpp",
			"Source/jsoncpp/*.cpp",
			"Source/SimpleFileWatcher/**.c",
			"Source/SimpleFileWatcher/**.cpp",
			"Source/*.c",
			"Source/*.cpp",
		}
		
		if os.target() == "windows" then
			excludes { "Source/SimpleFileWatcher/FileWatcherLinux.cpp", "Source/SimpleFileWatcher/FileWatcherOSX.cpp" }
		end
		
		if os.target() == "linux" then
			excludes { "Source/SimpleFileWatcher/FileWatcherWin32.cpp", "Source/SimpleFileWatcher/FileWatcherOSX.cpp" }
		end
		
		if os.target() == "macosx" then
			excludes { "Source/SimpleFileWatcher/FileWatcherWin32.cpp", "Source/SimpleFileWatcher/FileWatcherLinux.cpp" }
		end
		
		filter "configurations:Debug"
			defines { "DEBUG" }
			targetsuffix "d"
			symbols "On"
		
		filter "configurations:Release"
			defines { "NDEBUG" }
			optimize "On"
