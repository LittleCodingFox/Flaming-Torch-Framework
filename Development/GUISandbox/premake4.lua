
-- A solution contains projects, and defines the available configurations
solution "GUISandbox"
	configurations { "Debug", "Release" }
	platforms { "native" }
	
	-- A project defines one build target
	project "GUISandbox"
		kind "ConsoleApp"
		language "C++"
		files {
			"../Core/*.hpp",
			"../Core/*.cpp",
			"Source/**.hpp",
			"Source/**.cpp"
		}
		
		pchheader "FlamingCore.hpp"
		pchsource "../Core/FlamingCore.cpp"
		
		includedirs {
			"Source/",
			"../../Dependencies/Headers/",
			"../../Dependencies/Headers/lua/",
			"../../Dependencies/Headers/zlib/",
			"../../Dependencies/Source/angelscript/",
			"../../Dependencies/Source/webp/",
			"../Core/"
		}
		
		defines({ "SFML_WINDOW_EXPORTS", "SFML_SYSTEM_EXPORTS", "SFML_NETWORK_EXPORTS",
			"SFML_GRAPHICS_EXPORTS", "SFML_AUDIO_EXPORTS", "GLEW_STATIC", "UNICODE" })

		if os.get() == "windows" then
			buildoptions { "/Zm139", "/bigobj" }

			postbuildcommands {
				"finishbuild.bat \"$(TargetPath)\" \"$(SolutionDir)..\\..\\Content\\$(TargetFileName)\" " ..
				"\"$(SolutionDir)..\\..\\Binaries\\$(ProjectName)\\$(ConfigurationName)\\$(TargetName).pdb\" \"$(SolutionDir)..\\Tools\\MakePackages.cmd\" " ..
				"\"$(SolutionDir)..\\..\\Binaries\\Packer\\Release\\Packer.exe\" \"$(SolutionDir)Artwork\\PackageData\" \"$(SolutionDir)..\\..\\Content\""
			}
		
			libdirs {
				"../../Dependencies/Libs/Win32/"
			}
		end
		
		if os.get() == "macosx" then
			postbuildcommands {
				"sh finishbuild.sh \"../../Binaries/GUISandbox/Debug/GUISandbox\" \"../../Content/GUISandbox\" " ..
				"\"../Tools/MakePackages.sh\" \"../../Binaries/Packer/Debug/Packer\" \"Artwork/PackageData\" " ..
				" \"../../Content/\""
			}
			
			libdirs {
				"../../Dependencies/Libs/OSX/"
			}
				
			files { "../Core/FileSystem_OSX.mm" }
		end

		if os.get() == "linux" then
			postbuildcommands {
				"sh finishbuild.sh \"../../Binaries/GUISandbox/Debug/GUISandbox\" \"../../Content/GUISandbox\" " ..
				"\"../Tools/MakePackages.sh\" \"../../Binaries/Packer/Debug/Packer\" \"Artwork/PackageData\" " ..
				" \"../../Content/\""
			}
		end
 
		configuration "Debug"
			libdirs {
				"../../Binaries/FlamingDependencies/Debug/"
			}
		
			if os.get() == "windows" then
				defines({ "DEBUG", "_WIN32", "WIN32", "GLEW_STATIC" })

				links { "winmm", "ws2_32", "opengl32", "glu32", "glew", "FlamingDependenciesd", "jpeg", "openal32", "sndfile" }
			end
		
			if os.get() == "linux" then
				defines({ "DEBUG", "__LINUX__", "GLEW_STATIC" })
				links { "GL", "GLU", "GLEW", "FlamingDependenciesd", "jpeg", "openal", "sndfile", "X11", "pthread", "dl", "Xrandr" }
			end
		
			if os.get() == "macosx" then
				defines({
					"DEBUG", "__APPLE__"
				})

				links { "OpenGL.framework", "glew", "FlamingDependenciesd",
					"Foundation.framework", "AppKit.framework", "IOKit.framework", "Carbon.framework",
					"GLEW", "OpenAL.framework", "jpeg", "sndfile.framework"
				}
			end

			flags { "Symbols" }
 
		configuration "Release"
			libdirs {
				"../../Binaries/FlamingDependencies/Release/"
			}
		
			if os.get() == "windows" then
				defines({ "NDEBUG", "_WIN32", "WIN32" })
		
				libdirs {
					"../../Dependencies/Libs/Win32/"
				}

				links { "winmm", "ws2_32", "opengl32", "glu32", "glew", "FlamingDependencies", "jpeg", "openal32", "sndfile" }
			end
		
			if os.get() == "linux" then
				defines({ "NDEBUG", "__LINUX__" })
				links { "GL", "GLU", "GLEW", "FlamingDependenciesd", "jpeg", "openal", "sndfile", "X11", "pthread", "dl", "Xrandr" }
			end
		
			if os.get() == "macosx" then
				defines({ "NDEBUG", "__APPLE__" })
				
				links { "OpenGL.framework", "glew", "FlamingDependencies" }
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
