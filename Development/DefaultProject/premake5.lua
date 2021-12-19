include("conanbuildinfo.premake.lua")

workspace "DefaultProject"
	conan_basic_setup()

	configurations { "Debug", "Release" }
	
	project "DefaultProject"
		kind "ConsoleApp"
		language "C++"
		targetdir("../../Binaries/DefaultProject/%{cfg.buildcfg}")
		objdir("../../Temp/DefaultProject/%{cfg.buildcfg}")

		files {
			"../Core/**.hpp",
			"../Core/**.cpp",
			"Source/**.hpp",
			"Source/**.cpp"
		}
		
		includedirs {
			"Source/",
			"../../Dependencies/Headers/",
			"../Core/"
		}

		libdirs {
			"../../Binaries/FlamingDependencies/%{cfg.buildcfg}"
		}
		
		linkoptions { conan_exelinkflags }
		
		if os.target() == "windows" then
			buildoptions { "/bigobj" }
		end
		
		if os.target() == "linux" then
			pchheader "../Core/FlamingCore.hpp"
			pchsource "../Core/FlamingCore.cpp"
		elseif os.target() == "windows" then
			pchheader "FlamingCore.hpp"
			pchsource "../Core/FlamingCore.cpp"
		end
		
		if os.target() == "windows" then
			libdirs {
				"../../Dependencies/Libs/Win64/"
			}
		end
		
		if os.target() == "macosx" then
			libdirs {
				"../../Dependencies/Libs/OSX/"
			}
			
			linkoptions {
				"-F ../../Dependencies/Libs/OSX/"
			}
			
			files { "../Core/FileSystem_OSX.mm" }
		end

		filter "configurations:Debug"
			defines({ "DEBUG" })
			
			if os.target() == "windows" then
				links { "FlamingDependenciesd", "winmm", "ws2_32", "opengl32", "glu32", "libglew32" }
			end

			if os.target() == "macosx" then

				links { "OpenGL.framework", "FlamingDependenciesd", "Foundation.framework",
					"AppKit.framework", "IOKit.framework", "Carbon.framework",
					"OpenAL.framework"
				}
			end

			symbols "On"
 
		filter "configurations:Release"
			
			defines({ "NDEBUG" })
			
			if os.target() == "windows" then
				links { "FlamingDependencies", "winmm", "ws2_32", "opengl32", "glu32", "libglew32" }
			end
		
			if os.target() == "macosx" then
				links { "OpenGL.framework", "FlamingDependencies",
					"Foundation.framework", "AppKit.framework", "IOKit.framework", "Carbon.framework",
					"OpenAL.framework"
				}
			end

			optimize "On"
