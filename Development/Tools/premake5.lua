include("conanbuildinfo.premake.lua")

workspace "Tools"
	conan_basic_setup()

	configurations { "Debug", "Release" }
		
	if os.target() == "linux" then
		pchheader "../Core/FlamingCore.hpp"
		pchsource "../Core/FlamingCore.cpp"
	elseif os.target() == "windows" then
		pchheader "FlamingCore.hpp"
		pchsource "../Core/FlamingCore.cpp"
	end
	
	if os.target() == "windows" then
		buildoptions { "/bigobj" }
	end
	
	project "Packer"
		kind "ConsoleApp"
		language "C++"
		targetdir "../../Binaries/Packer/%{cfg.buildcfg}"
		objdir "../../Temp/Packer/%{cfg.buildcfg}"

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
			"../Core/"
		}
		
		linkoptions { conan_exelinkflags }
		
		libdirs {
			"../../Dependencies/Libs/",
			"../../Binaries/FlamingDependencies/%{cfg.buildcfg}"
		}
		
		if os.target() == "macosx" then
			libdirs {
				"../../Dependencies/Libs/OSX/"
			}
			
			files { "../Core/FileSystem_OSX.mm" }
			links { "Foundation.framework" }
		end
		 
		filter "configurations:Debug"
		
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
		targetdir "../../Binaries/TiledConverter/%{cfg.buildcfg}"
		objdir "../../Temp/TiledConverter/%{cfg.buildcfg}"

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
			"../Core/"
		}
		
		libdirs {
			"../../Dependencies/Libs/",
			"../../Binaries/FlamingDependencies/%{cfg.buildcfg}"
		}
		
		linkoptions { conan_exelinkflags }
		
		if os.target() == "macosx" then
			libdirs {
				"../../Dependencies/Libs/OSX/"
			}
				
			files { "../Core/FileSystem_OSX.mm" }
			links { "Foundation.framework" }
		end
 
		filter "configurations:Debug"
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
	
	project "Baker"
		kind "ConsoleApp"
		language "C++"
		targetdir "../../Binaries/Baker/%{cfg.buildcfg}"
		objdir "../../Temp/Baker/%{cfg.buildcfg}"

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
			"../Core/"
		}
		
		libdirs {
			"../../Dependencies/Libs/",
			"../../Binaries/FlamingDependencies/%{cfg.buildcfg}"
		}
		
		linkoptions { conan_exelinkflags }
		
		if os.target() == "macosx" then
			libdirs {
				"../../Dependencies/Libs/OSX/"
			}
				
			files { "../Core/FileSystem_OSX.mm" }
			links { "Foundation.framework" }
		end
 
		filter "configurations:Debug"
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
	
	project "TexturePacker"
		kind "ConsoleApp"
		language "C++"
		targetdir "../../Binaries/TexturePacker/%{cfg.buildcfg}"
		objdir "../../Temp/TexturePacker/%{cfg.buildcfg}"

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
			"../Core/"
		}
		
		libdirs {
			"../../Dependencies/Libs/",
			"../../Binaries/FlamingDependencies/%{cfg.buildcfg}"
		}
		
		linkoptions { conan_exelinkflags }
		
		if os.target() == "macosx" then
			libdirs {
				"../../Dependencies/Libs/OSX/"
			}
				
			files { "../Core/FileSystem_OSX.mm" }
			links { "Foundation.framework" }
		end
 
		filter "configurations:Debug"
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
