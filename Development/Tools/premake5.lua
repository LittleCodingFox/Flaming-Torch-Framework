
-- A solution contains projects, and defines the available configurations
solution "Tools"
	configurations { "Debug", "Release" }
	platforms { "x64" }
	
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
		
		if os.target() == "linux" then
			pchheader "../Core/FlamingCore.hpp"
			pchsource "../Core/FlamingCore.cpp"
		elseif os.target() == "windows" then
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
		
		if os.target() == "macosx" then
			libdirs {
				"../../Dependencies/Libs/OSX/"
			}
			
			files { "../Core/FileSystem_OSX.mm" }
			links { "Foundation.framework" }
		end

		buildoptions { "-std=c++20" }
		 
		filter "configurations:Debug"
			targetdir "../../Binaries/Packer/Debug"
			objdir "../../Temp/Packer/Debug"

			libdirs {
				"../../Binaries/FlamingDependencies/Debug/"
			}
		
			if os.target() == "windows" then
				defines({ "DEBUG", "_WIN32", "WIN32" })
				links { "winmm", "ws2_32", "FlamingDependenciesd" }
			end
		
			if os.target() == "linux" then
				defines({ "DEBUG", "__LINUX__" })
				links { "FlamingDependenciesd", "dl", "pthread" }
			end
		
			if os.target() == "macosx" then
				defines({ "DEBUG", "__APPLE__" })
				links { "FlamingDependenciesd" }
			end

			symbols "On"
 
		filter "configurations:Release"
			targetdir "../../Binaries/Packer/Release"
			objdir "../../Temp/Packer/Release"

			libdirs {
				"../../Binaries/FlamingDependencies/Release/"
			}

			if os.target() == "windows" then
				defines({ "NDEBUG", "_WIN32", "WIN32" })
				links { "winmm", "ws2_32", "FlamingDependencies" }
			end
		
			if os.target() == "linux" then
				defines({ "NDEBUG", "__LINUX__" })
				links { "FlamingDependencies", "dl", "pthread" }
			end
		
			if os.target() == "macosx" then
				defines({ "NDEBUG", "__APPLE__" })
				links { "FlamingDependencies" }
			end

			optimize "On"
	
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

		buildoptions { "-std=c++20" }
		
		if os.target() == "linux" then
			pchheader "../Core/FlamingCore.hpp"
			pchsource "../Core/FlamingCore.cpp"
		elseif os.target() == "windows" then
			pchheader "FlamingCore.hpp"
			pchsource "../Core/FlamingCore.cpp"
		end
		
		defines({
			"SFML_WINDOW_EXPORTS", "SFML_SYSTEM_EXPORTS", "SFML_NETWORK_EXPORTS",
			"SFML_GRAPHICS_EXPORTS", "SFML_AUDIO_EXPORTS", "GLEW_STATIC", "UNICODE"
		})
		
		if os.target() == "macosx" then
			libdirs {
				"../../Dependencies/Libs/OSX/"
			}
				
			files { "../Core/FileSystem_OSX.mm" }
			links { "Foundation.framework" }
		end
 
		filter "configurations:Debug"
			targetdir "../../Binaries/TiledConverter/Debug"
			objdir "../../Temp/TiledConverter/Debug"

			libdirs {
				"../../Binaries/FlamingDependencies/Debug/"
			}
		
			if os.target() == "windows" then
				defines({ "DEBUG", "_WIN32", "WIN32" })
				links { "winmm", "ws2_32", "FlamingDependenciesd" }
			end
		
			if os.target() == "linux" then
				defines({ "DEBUG", "__LINUX__" })
				links { "FlamingDependenciesd", "dl", "pthread" }
			end
		
			if os.target() == "macosx" then
				defines({ "DEBUG", "__APPLE__" })
				links { "FlamingDependenciesd" }
			end

			symbols "On"
 
		filter "configurations:Release"
			targetdir "../../Binaries/TiledConverter/Release"
			objdir "../../Temp/TiledConverter/Release"

			libdirs {
				"../../Binaries/FlamingDependencies/Release/"
			}

			if os.target() == "windows" then
				defines({ "NDEBUG", "_WIN32", "WIN32" })
				links { "winmm", "ws2_32", "FlamingDependencies" }
			end
		
			if os.target() == "linux" then
				defines({ "NDEBUG", "__LINUX__" })
				links { "FlamingDependencies", "dl", "pthread" }
			end
		
			if os.target() == "macosx" then
				defines({ "NDEBUG", "__APPLE__" })
				links { "FlamingDependencies" }
			end

			optimize "On"
	
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
		
		if os.target() == "linux" then
			pchheader "../Core/FlamingCore.hpp"
			pchsource "../Core/FlamingCore.cpp"
		elseif os.target() == "windows" then
			pchheader "FlamingCore.hpp"
			pchsource "../Core/FlamingCore.cpp"
		end
		
		defines({
			"SFML_WINDOW_EXPORTS", "SFML_SYSTEM_EXPORTS", "SFML_NETWORK_EXPORTS",
			"SFML_GRAPHICS_EXPORTS", "SFML_AUDIO_EXPORTS", "GLEW_STATIC", "UNICODE"
		})
		
		if os.target() == "macosx" then
			libdirs {
				"../../Dependencies/Libs/OSX/"
			}
				
			files { "../Core/FileSystem_OSX.mm" }
			links { "Foundation.framework" }
		end

		buildoptions { "-std=c++20" }
 
		filter "configurations:Debug"
			targetdir "../../Binaries/Baker/Debug"
			objdir "../../Temp/Baker/Debug"

			libdirs {
				"../../Binaries/FlamingDependencies/Debug/"
			}
		
			if os.target() == "windows" then
				defines({ "DEBUG", "_WIN32", "WIN32" })
				links { "winmm", "ws2_32", "FlamingDependenciesd" }
			end
		
			if os.target() == "linux" then
				defines({ "DEBUG", "__LINUX__" })
				links { "FlamingDependenciesd", "dl", "pthread" }
			end
		
			if os.target() == "macosx" then
				defines({ "DEBUG", "__APPLE__" })
				links { "FlamingDependenciesd" }
			end

			symbols "On"
 
		filter "configurations:Release"
			targetdir "../../Binaries/Baker/Release"
			objdir "../../Temp/Baker/Release"

			libdirs {
				"../../Binaries/FlamingDependencies/Release/"
			}

			if os.target() == "windows" then
				defines({ "NDEBUG", "_WIN32", "WIN32" })
				links { "winmm", "ws2_32", "FlamingDependencies" }
			end
		
			if os.target() == "linux" then
				defines({ "NDEBUG", "__LINUX__" })
				links { "FlamingDependencies", "dl", "pthread" }
			end
		
			if os.target() == "macosx" then
				defines({ "NDEBUG", "__APPLE__" })
				links { "FlamingDependencies" }
			end

			optimize "On"
	
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
		
		if os.target() == "linux" then
			pchheader "../Core/FlamingCore.hpp"
			pchsource "../Core/FlamingCore.cpp"
		elseif os.target() == "windows" then
			pchheader "FlamingCore.hpp"
			pchsource "../Core/FlamingCore.cpp"
		end
		
		defines({
			"SFML_WINDOW_EXPORTS", "SFML_SYSTEM_EXPORTS", "SFML_NETWORK_EXPORTS",
			"SFML_GRAPHICS_EXPORTS", "SFML_AUDIO_EXPORTS", "GLEW_STATIC", "UNICODE"
		})
		
		if os.target() == "macosx" then
			libdirs {
				"../../Dependencies/Libs/OSX/"
			}
				
			files { "../Core/FileSystem_OSX.mm" }
			links { "Foundation.framework" }
		end

		buildoptions { "-std=c++20" }
 
		filter "configurations:Debug"
			targetdir "../../Binaries/TexturePacker/Debug"
			objdir "../../Temp/TexturePacker/Debug"

			libdirs {
				"../../Binaries/FlamingDependencies/Debug/"
			}
		
			if os.target() == "windows" then
				defines({ "DEBUG", "_WIN32", "WIN32" })
				links { "winmm", "ws2_32", "FlamingDependenciesd" }
			end
		
			if os.target() == "linux" then
				defines({ "DEBUG", "__LINUX__" })
				links { "FlamingDependenciesd", "dl", "pthread" }
			end
		
			if os.target() == "macosx" then
				defines({ "DEBUG", "__APPLE__" })
				links { "FlamingDependenciesd" }
			end

			symbols "On"
 
		filter "configurations:Release"
			targetdir "../../Binaries/TexturePacker/Release"
			objdir "../../Temp/TexturePacker/Release"

			libdirs {
				"../../Binaries/FlamingDependencies/Release/"
			}

			if os.target() == "windows" then
				defines({ "NDEBUG", "_WIN32", "WIN32" })
				links { "winmm", "ws2_32", "FlamingDependencies" }
			end
		
			if os.target() == "linux" then
				defines({ "NDEBUG", "__LINUX__" })
				links { "FlamingDependencies", "dl", "pthread" }
			end
		
			if os.target() == "macosx" then
				defines({ "NDEBUG", "__APPLE__" })
				links { "FlamingDependencies" }
			end

			optimize "On"
