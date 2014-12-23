
-- A solution contains projects, and defines the available configurations
solution "DefaultProject"
	configurations { "Debug", "Release" }
	platforms { "x32", "x64" }
	
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
		
		if os.get() == "linux" then
			pchheader "../Core/FlamingCore.hpp"
			pchsource "../Core/FlamingCore.cpp"
		elseif os.get() == "windows" then
			pchheader "FlamingCore.hpp"
			pchsource "../Core/FlamingCore.cpp"
		end
		
		if os.get() == "windows" then
			buildoptions { "/Zm200", "/bigobj" }
			postbuildcommands {
				"finishbuild.bat \"$(TargetPath)\" \"$(SolutionDir)..\\..\\Content\\$(TargetName)\\$(TargetFileName)\" \"$(SolutionDir)..\\..\\Binaries\\$(ProjectName)\\$(ConfigurationName)\\$(TargetName).pdb\""
			}
		
			libdirs {
				"../../Dependencies/Libs/Win32/"
			}
		else
			buildoptions { "-w" }
		end
		
		if os.get() == "macosx" then
			postbuildcommands {
				"sh finishbuild.sh \"../../Binaries/DefaultProject/Debug/DefaultProject\" \"../../Content/DefaultProject/DefaultProject\" "
			}
			
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


-- From http://industriousone.com/topic/how-get-current-configuration
-- iterate over all solutions
for sln in premake.solution.each() do
   -- iterate over all projects in the solution
	for pi = 1, #sln.projects do
		-- make this the active project
		prj = sln.projects[pi]
		project(prj.name)
 
		-- iterate over all configurations
		for ci = 1, #sln.configurations do
			-- make this the active configuration
			cfgname = sln.configurations[ci]
			configuration(cfgname)
 
			-- do my custom stuff
			targetdir(path.join(prj.basedir, "../../Binaries/" .. prj.name .. "/" .. cfgname))
			objdir(path.join(prj.basedir, "../../Temp/" .. prj.name .. "/" .. cfgname))
		end
	end
end
