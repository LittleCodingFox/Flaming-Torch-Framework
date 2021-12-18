
-- A solution contains projects, and defines the available configurations
solution "DefaultProject"
	configurations { "Debug", "Release" }
	platforms { "x64" }
	
	-- A project defines one build target
	project "DefaultProject"
		kind "ConsoleApp"
		language "C++"
		files {
			"../Core/**.hpp",
			"../Core/**.cpp",
			"Source/**.hpp",
			"Source/**.cpp"
		}
		
		includedirs {
			"Source/",
			"../../Dependencies/Headers/",
			"../../Dependencies/Headers/lua/",
			"../../Dependencies/Headers/zlib/",
			"../../Dependencies/Source/angelscript/",
			"../../Dependencies/Source/webp/",
			"../Core/"
		}
		
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
		
		defines({ "SFML_WINDOW_EXPORTS", "SFML_SYSTEM_EXPORTS", "SFML_NETWORK_EXPORTS",
			"SFML_GRAPHICS_EXPORTS", "SFML_AUDIO_EXPORTS", "GLEW_STATIC", "UNICODE" })
 
		buildoptions { "-std=c++20" }

		filter "configurations:Debug"
			libdirs {
				"../../Binaries/FlamingDependencies/Debug/"
			}
			
			objdir("../../Temp/DefaultProject/Debug/")
			targetdir("../../Binaries/DefaultProject/Debug/")
			
			defines({ "DEBUG" })
		
			if os.target() == "windows" then
				defines({ "_WIN32", "WIN32" })
				
				buildoptions { "/bigobj" }

				links { "FlamingDependenciesd", "winmm", "ws2_32", "opengl32", "glu32", "glew32s", "openal32", "sndfile" }
			end
		
			if os.target() == "linux" then
				defines({ "__LINUX__" })
				links { "GL", "GLU", "GLEW", "FlamingDependenciesd", "Xrandr", "X11", "pthread", "dl", "openal", "sndfile", "udev" }
			end
		
			if os.target() == "macosx" then
				defines({ "__APPLE__" })

				links { "OpenGL.framework", "glew", "FlamingDependenciesd",
					"Foundation.framework", "AppKit.framework", "IOKit.framework", "Carbon.framework",
					"GLEW", "OpenAL.framework", "../../Dependencies/Libs/OSX/sndfile.framework"
				}
			end

			symbols "On"
 
		filter "configurations:Release"
			libdirs {
				"../../Binaries/FlamingDependencies/Release/"
			}
			
			objdir("../../Temp/DefaultProject/Release/")
			targetdir("../../Binaries/DefaultProject/Release/")
			
			defines({ "NDEBUG" })
			
			if os.target() == "windows" then
				defines({ "_WIN32", "WIN32" })
				
				buildoptions { "/bigobj" }

				links { "FlamingDependencies", "winmm", "ws2_32", "opengl32", "glu32", "glew", "openal32", "sndfile" }
			end
		
			if os.target() == "linux" then
				defines({ "__LINUX__" })
				links { "GL", "GLU", "GLEW", "FlamingDependencies", "Xrandr", "X11", "pthread", "dl", "openal", "sndfile", "udev" }
			end
		
			if os.target() == "macosx" then
				defines({ "__APPLE__" })
				
				links { "OpenGL.framework", "glew", "FlamingDependencies",
					"Foundation.framework", "AppKit.framework", "IOKit.framework", "Carbon.framework",
					"GLEW", "OpenAL.framework", "sndfile.framework"
				}
			end

			optimize "On"
