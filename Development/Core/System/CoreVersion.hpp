#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>
#include "Platform.hpp"
#include "Common.hpp"
#include <string.h>
#include <string>
#include <memory>
#include <map>
#include <math.h>
#include <SimpleDelegate/SimpleDelegate.hpp>

#define FTSTD_VERSION_MAJOR 0
#define FTSTD_VERSION_MINOR 11
#define FLAMINGRENDER_VERSION_ID 'fr01'

//Subsystem Priorities
#define LOG_PRIORITY 1
#define GAMECLOCK_PRIORITY 2
#define FPSCOUNTER_PRIORITY 3
#define LUASCRIPTMANAGER_PRIORITY 4
#define CONSOLE_PRIORITY 5
#define RENDERERMANAGER_PRIORITY 6
#define RESOURCEMANAGER_PRIORITY 7
#define OBJECTMODEL_PRIORITY 8
#define NETWORK_PRIORITY 14
#define SOUNDMANAGER_PRIORITY 15
#define FILESYSTEM_WATCHER_PRIORITY 60
#define PACKAGE_FILESYSTEM_PRIORITY 61
#define PROFILER_PRIORITY 62
#define FUTURE_PRIORITY 63

namespace FlamingTorch
{
	typedef uint8_t uint8;
	typedef int8_t int8;
	typedef uint16_t uint16;
	typedef int16_t int16;
	typedef uint32_t uint32;
	typedef int32_t int32;
	typedef uint64_t uint64;
	typedef int64_t int64;
	typedef float f32;
	typedef double f64;

	class Rect;
	class Vector2;
	typedef uint32 VersionType;

	namespace PlatformType
	{
		enum PlatformType
		{
			PC,
			Mobile
		};
	};

	namespace ScreenRotation
	{
		enum ScreenRotation
		{
			North = 0, //!<Default
			South, //!<Vertical Flipped
			East, //!<90 degree right
			West //!<90 degree left
		};
	};

	class PlatformInfo
	{
	public:
		static uint8 PlatformType;

		static uint8 ScreenRotation;

		static uint32 ResolutionOverrideWidth;
		static uint32 ResolutionOverrideHeight;

		static Rect RotateScreen(const Rect &In);
		static uint8 ScreenOrientation(const Rect &In);
		static Rect ToNorthRotation(const Rect &In);
		static Vector2 ScreenSize(const Rect &In);
		static f32 ScreenRotationValue();
	};

	/*!
	*	Core Utilities
	*/
	class CoreUtils
	{
	public:
		/*!
		*	\return a string with the name of this platform
		*	\param IncludeVersion whether to include version info
		*/
		static std::string PlatformString(bool IncludeVersion = false);

		/*!
		*	Runs a program from the OS
		*	\param ExePath the path to the executable file
		*	\param Parameters the parameters to pass to the executable
		*	\param WorkingDirectory the directory from which to run the executable
		*	\return the return code from running the .exe, or -1 if there was a problem running it
		*	\note on non-Windows systems this always returns 0 for now
		*/
		static int32 RunProgram(const std::string &ExePath, const std::string &Parameters, const std::string &WorkingDirectory);

		/*!
		*	Makes a VersionType from two bytes
		*	\param a the major version
		*	\param b the minor version
		*/
		static inline VersionType MakeVersion(uint8 a, uint8 b)
		{
			VersionType Result = 0;
			uint8 *t = (uint8*)&Result;
			t[0] = a;
			t[1] = b;

			return Result;
		};

		/*!
		*	Makes a version string from two bytes
		*	\param a the major version
		*	\param b the minor version
		*/
		static std::string MakeVersionString(uint8 a, uint8 b);

		/*!
		*	Makes an Int from Bytes
		*	\param a the first byte
		*	\param b the second byte
		*	\param c the third byte
		*	\param d the fourth byte
		*/
		static inline int32 MakeIntFromBytes(uint8 a, uint8 b, uint8 c, uint8 d)
		{
			int32 Result = 0;
			uint8 *t = (uint8*)&Result;
			t[0] = a;
			t[1] = b;
			t[2] = c;
			t[3] = d;

			return Result;
		};

		/*!
		*	Makes an Int from Bytes
		*	\param Bytes an array of uint8
		*	\param Length the amount of bytes (max 4)
		*/
		static inline int32 MakeIntFromBytes(uint8 *Bytes, uint32 Length = 4)
		{
			int32 Result = 0;
			uint8 *t = (uint8*)&Result;
			t[0] = Bytes[0];

			if(Length > 1)
			{
				t[1] = Bytes[1];

				if(Length > 2)
				{
					t[2] = Bytes[2];

					if(Length > 3)
					{
						t[3] = Bytes[3];
					};
				};
			};

			return Result;
		};

		/*!
		*	\return the current desktop work area (position and size)
		*/
		static Rect GetDesktopWorkArea();
	};

	void LibWarning(unsigned int Line, const char* FileName,
		const char* Function, const char* Variable, const char *Message, ...);
};
