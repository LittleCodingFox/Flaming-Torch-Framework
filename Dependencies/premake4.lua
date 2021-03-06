-- A solution contains projects, and defines the available configurations
solution "Dependencies"
	configurations { "Debug", "Release" }
	platforms { "x32", "x64" }
	
	-- A project defines one build target
	project "FlamingDependencies"
		kind "StaticLib"
		language "C++"
		
		defines({
			"FT2_BUILD_LIBRARY"
		})
		
		files {
			"Source/base64/**.c",
			"Source/base64/**.cpp",
			"Source/enet/**.c",
			"Source/enet/**.cpp",
			"Source/jsoncpp/**.c",
			"Source/jsoncpp/**.cpp",
			"Source/libpng/**.c",
			"Source/libpng/**.cpp",
			"Source/lua/**.c",
			"Source/lua/**.cpp",
			"Source/luabind/**.c",
			"Source/luabind/**.cpp",
			"Source/SimpleFileWatcher/**.c",
			"Source/SimpleFileWatcher/**.cpp",
			"Source/tb/**.cpp",
			"Source/tinyxml/**.c",
			"Source/tinyxml/**.cpp",
			"Source/webp/**.c",
			"Source/webp/**.cpp",
			"Source/zlib/**.c",
			"Source/zlib/**.cpp",
			"Source/SFML/SFML/Audio/*.cpp",
			"Source/SFML/SFML/Graphics/*.cpp",
			"Source/SFML/SFML/System/*.cpp",
			"Source/SFML/SFML/Window/*.cpp",
			"Source/SFML/SFML/Network/*.cpp",
			"Source/*.c",
			"Source/*.cpp",
		}
		
		includedirs {
			"Headers/",
			"Headers/lua/",
			"Headers/zlib/",
			"Source/webp/",
			"Source/SFML/",
			"Source/SFML/Headers/AL/",
			"Source/SFML/Headers/jpeg/"
		}

		excludes {
			"Source/lua/luac.c", 
			"Source/lua/lua.c",
			"Source/SFML/SFML/Window/EGL*.cpp",
			"Source/SFML/SFML/Window/Egl*.cpp"
		}
		
		files {
			"Source/freetype/autofit/autofit.c",
			"Source/freetype/base/ftsystem.c",
			"Source/freetype/base/ftinit.c",
			"Source/freetype/base/ftdebug.c",
			"Source/freetype/base/ftbase.c",
			"Source/freetype/base/ftbbox.c",
			"Source/freetype/base/ftglyph.c",
			"Source/freetype/base/ftbdf.c",
			"Source/freetype/base/ftbitmap.c",
			"Source/freetype/base/ftcid.c",
			"Source/freetype/base/ftfstype.c",
			"Source/freetype/base/ftgasp.c",
			"Source/freetype/base/ftgxval.c",
			"Source/freetype/base/ftlcdfil.c",
			"Source/freetype/base/ftmm.c",
			"Source/freetype/base/ftotval.c",
			"Source/freetype/base/ftpatent.c",
			"Source/freetype/base/ftpfr.c",
			"Source/freetype/base/ftstroke.c",
			"Source/freetype/base/ftsynth.c",
			"Source/freetype/base/fttype1.c",
			"Source/freetype/base/ftwinfnt.c",
			"Source/freetype/base/ftxf86.c",
			"Source/freetype/bdf/bdf.c",
			"Source/freetype/bzip2/ftbzip2.c",
			"Source/freetype/cache/ftcache.c",
			"Source/freetype/cff/cff.c",
			"Source/freetype/cid/type1cid.c",
			"Source/freetype/gxvalid/gxvalid.c",
			"Source/freetype/gzip/ftgzip.c",
			"Source/freetype/lzw/ftlzw.c",
			"Source/freetype/otvalid/otvalid.c",
			"Source/freetype/pcf/pcf.c",
			"Source/freetype/pfr/pfr.c",
			"Source/freetype/psaux/psaux.c",
			"Source/freetype/pshinter/pshinter.c",
			"Source/freetype/psnames/psnames.c",
			"Source/freetype/raster/ftraster.c",
			"Source/freetype/raster/raster.c",
			"Source/freetype/sfnt/sfnt.c",
			"Source/freetype/smooth/smooth.c",
			"Source/freetype/truetype/truetype.c",
			"Source/freetype/type1/type1.c",
			"Source/freetype/type42/type42.c",
			"Source/freetype/winfonts/winfnt.c"
		}
		
		libdirs {
			"Libs/"
		}
		
		defines({
			"SFML_WINDOW_EXPORTS", "SFML_SYSTEM_EXPORTS", "SFML_NETWORK_EXPORTS",
			"SFML_GRAPHICS_EXPORTS", "SFML_AUDIO_EXPORTS", "GLEW_STATIC", "UNICODE"
		})

		if os.get() == "linux" then
			files {
				"Source/SFML/SFML/Network/Unix/*.cpp",
				"Source/SFML/SFML/System/Unix/*.cpp",
				"Source/SFML/SFML/Window/Linux/*.cpp",
				"Source/SFML/SFML/Window/Unix/SensorImpl.cpp"
			}
		end

		if os.get() == "linux" or os.get() == "macosx" then
			buildoptions { "-std=c++11" }
		end
		
		if os.get() == "windows" then
			excludes { "Source/SimpleFileWatcher/FileWatcherLinux.cpp", "Source/SimpleFileWatcher/FileWatcherOSX.cpp" }
			defines({ "_WIN32", "WIN32" })

			files {
				"Source/SFML/SFML/Network/Win32/*.cpp",
				"Source/SFML/SFML/System/Win32/*.cpp",
				"Source/SFML/SFML/Window/Win32/*.cpp"
			}

			includedirs {
				"Source/SFML/Headers/libsndfile/windows/"
			}
		end
		
		if os.get() == "linux" then
			excludes { "Source/SimpleFileWatcher/FileWatcherWin32.cpp", "Source/SimpleFileWatcher/FileWatcherOSX.cpp" }
			defines({ "__LINUX__" })

			includedirs {
				"Source/SFML/Headers/libsndfile/osx/"
			}
		end
		
		configuration "Debug"
			targetsuffix "d"
			
			defines({ "DEBUG" })
			
			targetdir("../Binaries/FlamingDependencies/Debug/")
			objdir("../Temp/FlamingDependencies/Debug/")

			flags { "Symbols" }
 
		configuration "Release"
			defines({ "NDEBUG" })
			
			targetdir("../Binaries/FlamingDependencies/Release/")
			objdir("../Temp/FlamingDependencies/Release/")

			flags { "Optimize" }
		
		configuration "macosx"
			files { "Source/freetype/base/ftmac.c" }

			files {
				"Source/SFML/SFML/Network/Unix/*.cpp",
				"Source/SFML/SFML/System/Unix/*.cpp",
				"Source/SFML/SFML/Window/OSX/*.cpp",
				"Source/SFML/SFML/Window/OSX/*.mm",
				"Source/SFML/SFML/Window/OSX/*.m"
			}

			includedirs {
				"Source/SFML/Headers/libsndfile/osx/"
			}
			
			excludes { "Source/SimpleFileWatcher/FileWatcherWin32.cpp", "Source/SimpleFileWatcher/FileWatcherLinux.cpp" }
			defines({ "NDEBUG", "__APPLE__", "__MACOSX__" })
			flags { "Optimize" }
