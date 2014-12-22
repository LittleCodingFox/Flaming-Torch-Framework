
-- A solution contains projects, and defines the available configurations
solution "Tools"
	configurations { "Debug", "Release" }
	platforms { "x32", "x64" }
	
	-- A project defines one build target
	project "Packer"
		kind "ConsoleApp"
		language "C++"
		files {
			"../Core/**.hpp",
			"../Core/**.cpp",
			"../../Dependencies/Headers/**.hpp",
			"Packer/**.hpp",
			"Packer/**.cpp"
		}
		
		includedirs {
			"Packer/",
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
		
		excludes {
			"../../Dependencies/Source/lua/luac.c", 
			"../../Dependencies/Source/lua/lua.c"
		}
		
		libdirs {
			"../../Dependencies/Libs/"
		}
		
		defines({
			"SFML_WINDOW_EXPORTS", "SFML_SYSTEM_EXPORTS", "SFML_NETWORK_EXPORTS",
			"SFML_GRAPHICS_EXPORTS", "SFML_AUDIO_EXPORTS", "GLEW_STATIC", "UNICODE"
		})
		
		if os.get() == "windows" then
			buildoptions { "/Zm139", "/bigobj" }
		else
			buildoptions { "-w" }
		end
		
		if os.get() == "macosx" then
			libdirs {
				"../../Dependencies/Libs/OSX/"
			}
			
			files { "../Core/FileSystem_OSX.mm" }
			links { "Foundation.framework" }
		end
		 
		configuration "Debug"
			libdirs {
				"../../Binaries/FlamingDependencies/Debug/"
			}
		
			if os.get() == "windows" then
				defines({ "DEBUG", "_WIN32", "WIN32" })
				links { "winmm", "ws2_32", "FlamingDependenciesd" }
			end
		
			if os.get() == "linux" then
				defines({ "DEBUG", "__LINUX__" })
				links { "FlamingDependenciesd", "dl", "pthread" }
				
				buildoptions { "-std=c++11" }
			end
		
			if os.get() == "macosx" then
				defines({ "DEBUG", "__APPLE__" })
				links { "FlamingDependenciesd" }

				buildoptions { "-std=c++11" }
			end

			flags { "Symbols" }
 
		configuration "Release"
			libdirs {
				"../../Binaries/FlamingDependencies/Release/"
			}

			if os.get() == "windows" then
				defines({ "NDEBUG", "_WIN32", "WIN32" })
				links { "winmm", "ws2_32", "FlamingDependencies" }
			end
		
			if os.get() == "linux" then
				defines({ "NDEBUG", "__LINUX__" })
				links { "FlamingDependencies", "dl", "pthread" }

				buildoptions { "-std=c++11" }
			end
		
			if os.get() == "macosx" then
				defines({ "NDEBUG", "__APPLE__" })
				links { "FlamingDependencies" }

				buildoptions { "-std=c++11" }
			end

			flags { "Optimize" }
	
	-- A project defines one build target
	project "TiledConverter"
		kind "ConsoleApp"
		language "C++"
		files {
			"../Core/**.hpp",
			"../Core/**.cpp",
			"../../Dependencies/Headers/**.hpp",
			"TiledConverter/**.h",
			"TiledConverter/**.cpp"
		}
		
		includedirs {
			"TiledConverter/",
			"../../Dependencies/Headers/",
			"../../Dependencies/Headers/lua/",
			"../../Dependencies/Headers/zlib/",
			"../../Dependencies/Source/angelscript/",
			"../../Dependencies/Source/webp/",
			"../Core/"
		}

		excludes {
			"../../Dependencies/Source/lua/luac.c", 
			"../../Dependencies/Source/lua/lua.c"
		}
		
		libdirs {
			"../../Dependencies/Libs/"
		}
		
		if os.get() == "linux" then
			pchheader "../Core/FlamingCore.hpp"
			pchsource "../Core/FlamingCore.cpp"
		elseif os.get() == "windows" then
			pchheader "FlamingCore.hpp"
			pchsource "../Core/FlamingCore.cpp"
		end
		
		defines({
			"SFML_WINDOW_EXPORTS", "SFML_SYSTEM_EXPORTS", "SFML_NETWORK_EXPORTS",
			"SFML_GRAPHICS_EXPORTS", "SFML_AUDIO_EXPORTS", "GLEW_STATIC", "UNICODE"
		})
		
		if os.get() == "windows" then
			buildoptions { "/Zm139", "/bigobj" }
		else
			buildoptions { "-w" }
		end
		
		if os.get() == "macosx" then
			libdirs {
				"../../Dependencies/Libs/OSX/"
			}
				
			files { "../Core/FileSystem_OSX.mm" }
			links { "Foundation.framework" }
		end
 
		configuration "Debug"
			libdirs {
				"../../Binaries/FlamingDependencies/Debug/"
			}
		
			if os.get() == "windows" then
				defines({ "DEBUG", "_WIN32", "WIN32" })
				links { "winmm", "ws2_32", "FlamingDependenciesd" }
			end
		
			if os.get() == "linux" then
				defines({ "DEBUG", "__LINUX__" })
				links { "FlamingDependenciesd", "dl", "pthread" }

				buildoptions { "-std=c++11" }
			end
		
			if os.get() == "macosx" then
				defines({ "DEBUG", "__APPLE__" })
				links { "FlamingDependenciesd" }

				buildoptions { "-std=c++11" }
			end

			flags { "Symbols" }
 
		configuration "Release"
			libdirs {
				"../../Binaries/FlamingDependencies/Release/"
			}

			if os.get() == "windows" then
				defines({ "NDEBUG", "_WIN32", "WIN32" })
				links { "winmm", "ws2_32", "FlamingDependencies" }
			end
		
			if os.get() == "linux" then
				defines({ "NDEBUG", "__LINUX__" })
				links { "FlamingDependencies", "dl", "pthread" }

				buildoptions { "-std=c++11" }
			end
		
			if os.get() == "macosx" then
				defines({ "NDEBUG", "__APPLE__" })
				links { "FlamingDependencies" }

				buildoptions { "-std=c++11" }
			end

			flags { "Optimize" }
	
	-- A project defines one build target
	project "Baker"
		kind "ConsoleApp"
		language "C++"
		files {
			"../Core/**.hpp",
			"../Core/**.cpp",
			"../../Dependencies/Headers/**.hpp",
			"Baker/**.h",
			"Baker/**.cpp"
		}
		
		includedirs {
			"Baker/",
			"../../Dependencies/Headers/",
			"../../Dependencies/Headers/lua/",
			"../../Dependencies/Headers/zlib/",
			"../../Dependencies/Source/angelscript/",
			"../../Dependencies/Source/webp/",
			"../Core/"
		}

		excludes {
			"../../Dependencies/Source/lua/luac.c", 
			"../../Dependencies/Source/lua/lua.c"
		}
		
		libdirs {
			"../../Dependencies/Libs/"
		}
		
		if os.get() == "linux" then
			pchheader "../Core/FlamingCore.hpp"
			pchsource "../Core/FlamingCore.cpp"
		elseif os.get() == "windows" then
			pchheader "FlamingCore.hpp"
			pchsource "../Core/FlamingCore.cpp"
		end
		
		defines({
			"SFML_WINDOW_EXPORTS", "SFML_SYSTEM_EXPORTS", "SFML_NETWORK_EXPORTS",
			"SFML_GRAPHICS_EXPORTS", "SFML_AUDIO_EXPORTS", "GLEW_STATIC", "UNICODE"
		})
		
		if os.get() == "windows" then
			buildoptions { "/Zm139", "/bigobj" }
		else
			buildoptions { "-w" }
		end
		
		if os.get() == "macosx" then
			libdirs {
				"../../Dependencies/Libs/OSX/"
			}
				
			files { "../Core/FileSystem_OSX.mm" }
			links { "Foundation.framework" }
		end
 
		configuration "Debug"
			libdirs {
				"../../Binaries/FlamingDependencies/Debug/"
			}
		
			if os.get() == "windows" then
				defines({ "DEBUG", "_WIN32", "WIN32" })
				links { "winmm", "ws2_32", "FlamingDependenciesd" }
			end
		
			if os.get() == "linux" then
				defines({ "DEBUG", "__LINUX__" })
				links { "FlamingDependenciesd", "dl", "pthread" }

				buildoptions { "-std=c++11" }
			end
		
			if os.get() == "macosx" then
				defines({ "DEBUG", "__APPLE__" })
				links { "FlamingDependenciesd" }
				buildoptions { "-std=c++11" }
			end

			flags { "Symbols" }
 
		configuration "Release"
			libdirs {
				"../../Binaries/FlamingDependencies/Release/"
			}

			if os.get() == "windows" then
				defines({ "NDEBUG", "_WIN32", "WIN32" })
				links { "winmm", "ws2_32", "FlamingDependencies" }
			end
		
			if os.get() == "linux" then
				defines({ "NDEBUG", "__LINUX__" })
				links { "FlamingDependencies", "dl", "pthread" }

				buildoptions { "-std=c++11" }
			end
		
			if os.get() == "macosx" then
				defines({ "NDEBUG", "__APPLE__" })
				links { "FlamingDependencies" }
				buildoptions { "-std=c++11" }
			end

			flags { "Optimize" }
	
	-- A project defines one build target
	project "TexturePacker"
		kind "ConsoleApp"
		language "C++"
		files {
			"../Core/**.hpp",
			"../Core/**.cpp",
			"../../Dependencies/Headers/**.hpp",
			"TexturePacker/**.h",
			"TexturePacker/**.cpp"
		}
		
		includedirs {
			"TexturePacker/",
			"../../Dependencies/Headers/",
			"../../Dependencies/Headers/lua/",
			"../../Dependencies/Headers/zlib/",
			"../../Dependencies/Source/angelscript/",
			"../../Dependencies/Source/webp/",
			"../Core/"
		}

		excludes {
			"../../Dependencies/Source/lua/luac.c", 
			"../../Dependencies/Source/lua/lua.c"
		}
		
		libdirs {
			"../../Dependencies/Libs/"
		}
		
		if os.get() == "linux" then
			pchheader "../Core/FlamingCore.hpp"
			pchsource "../Core/FlamingCore.cpp"
		elseif os.get() == "windows" then
			pchheader "FlamingCore.hpp"
			pchsource "../Core/FlamingCore.cpp"
		end
		
		defines({
			"SFML_WINDOW_EXPORTS", "SFML_SYSTEM_EXPORTS", "SFML_NETWORK_EXPORTS",
			"SFML_GRAPHICS_EXPORTS", "SFML_AUDIO_EXPORTS", "GLEW_STATIC", "UNICODE"
		})
		
		if os.get() == "windows" then
			buildoptions { "/Zm139", "/bigobj" }
		else
			buildoptions { "-w" }
		end
		
		if os.get() == "macosx" then
			libdirs {
				"../../Dependencies/Libs/OSX/"
			}
				
			files { "../Core/FileSystem_OSX.mm" }
			links { "Foundation.framework" }
		end
 
		configuration "Debug"
			libdirs {
				"../../Binaries/FlamingDependencies/Debug/"
			}
		
			if os.get() == "windows" then
				defines({ "DEBUG", "_WIN32", "WIN32" })
				links { "winmm", "ws2_32", "FlamingDependenciesd" }
			end
		
			if os.get() == "linux" then
				defines({ "DEBUG", "__LINUX__" })
				links { "FlamingDependenciesd", "dl", "pthread" }

				buildoptions { "-std=c++11" }
			end
		
			if os.get() == "macosx" then
				defines({ "DEBUG", "__APPLE__" })
				links { "FlamingDependenciesd" }

				buildoptions { "-std=c++11" }
			end

			flags { "Symbols" }
 
		configuration "Release"
			libdirs {
				"../../Binaries/FlamingDependencies/Release/"
			}

			if os.get() == "windows" then
				defines({ "NDEBUG", "_WIN32", "WIN32" })
				links { "winmm", "ws2_32", "FlamingDependencies" }
			end
		
			if os.get() == "linux" then
				defines({ "NDEBUG", "__LINUX__" })
				links { "FlamingDependencies", "dl", "pthread" }

				buildoptions { "-std=c++11" }
			end
		
			if os.get() == "macosx" then
				defines({ "NDEBUG", "__APPLE__" })
				links { "FlamingDependencies" }

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
