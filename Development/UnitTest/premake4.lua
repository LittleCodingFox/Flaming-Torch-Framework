
-- A solution contains projects, and defines the available configurations
solution "UnitTest"
	configurations { "Debug", "Release" }
	platforms { "x32", "x64" }
	
	-- A project defines one build target
	project "UnitTest"
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
		
		if os.get() == "linux" then
			pchheader "../Core/FlamingCore.hpp"
			pchsource "../Core/FlamingCore.cpp"
		elseif os.get() == "windows" then
			pchheader "FlamingCore.hpp"
			pchsource "../Core/FlamingCore.cpp"
		end
		
		if os.get() == "windows" then
			buildoptions { "/Zm200", "/bigobj" }
		
			libdirs {
				"../../Dependencies/Libs/Win32/"
			}
		else
			buildoptions { "-w" }
		end
		
		if os.get() == "macosx" then
			
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
 
		configuration "Debug"
			libdirs {
				"../../Binaries/FlamingDependencies/Debug/"
			}
			
			objdir("../../Temp/UnitTest/Debug/")
			targetdir("../../Binaries/UnitTest/Debug/")
			
			defines({ "DEBUG" })
		
			if os.get() == "windows" then
				defines({ "_WIN32", "WIN32" })

				links { "winmm", "ws2_32", "opengl32", "glu32", "glew", "FlamingDependenciesd", "jpeg", "openal32", "sndfile" }
			end
		
			if os.get() == "linux" then
				defines({ "__LINUX__" })
				links { "GL", "GLU", "GLEW", "FlamingDependenciesd", "Xrandr", "X11", "pthread", "dl", "openal", "jpeg", "sndfile", "udev" }

				buildoptions { "-std=c++11" }
			end
		
			if os.get() == "macosx" then
				defines({ "__APPLE__" })

				links { "OpenGL.framework", "glew", "FlamingDependenciesd",
					"Foundation.framework", "AppKit.framework", "IOKit.framework", "Carbon.framework",
					"GLEW", "OpenAL.framework", "jpeg", "../../Dependencies/Libs/OSX/sndfile.framework"
				}

				buildoptions { "-std=c++11" }
			end

			flags { "Symbols" }
 
		configuration "Release"
			libdirs {
				"../../Binaries/FlamingDependencies/Release/"
			}
			
			objdir("../../Temp/UnitTest/Release/")
			targetdir("../../Binaries/UnitTest/Release/")
			
			defines({ "NDEBUG" })
			
			if os.get() == "windows" then
				defines({ "_WIN32", "WIN32" })
		
				libdirs {
					"../../Dependencies/Libs/Win32/"
				}

				links { "winmm", "ws2_32", "opengl32", "glu32", "glew", "FlamingDependencies", "jpeg", "openal32", "sndfile" }
			end
		
			if os.get() == "linux" then
				defines({ "__LINUX__" })
				links { "GL", "GLU", "GLEW", "FlamingDependencies", "Xrandr", "X11", "pthread", "dl", "openal", "jpeg", "sndfile", "udev" }

				buildoptions { "-std=c++11" }
			end
		
			if os.get() == "macosx" then
				defines({ "__APPLE__" })
				
				links { "OpenGL.framework", "glew", "FlamingDependencies",
					"Foundation.framework", "AppKit.framework", "IOKit.framework", "Carbon.framework",
					"GLEW", "OpenAL.framework", "jpeg", "sndfile.framework"
				}

				buildoptions { "-std=c++11" }
			end

			flags { "Optimize" }
