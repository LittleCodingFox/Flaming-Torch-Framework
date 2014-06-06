#define _CRT_SECURE_NO_WARNINGS
#include "Platform.hpp"
#include "Common.hpp"
#include <string.h>
#include <string>
#include <map>
#include <math.h>
#include "Signal.h"
using namespace Gallant;

#define FTSTD_VERSION_MAJOR 0
#define FTSTD_VERSION_MINOR 10
#define FLAMINGRENDER_VERSION_ID 'fr01'

//Subsystem Priorities
#define LOG_PRIORITY 1
#define GAMECLOCK_PRIORITY 2
#define FPSCOUNTER_PRIORITY 3
#define LUASCRIPTMANAGER_PRIORITY 4
#define CONSOLE_PRIORITY 5
#define RENDERERMANAGER_PRIORITY 6
#define RESOURCEMANAGER_PRIORITY 7
#define NETWORK_PRIORITY 14
#define SOUNDMANAGER_PRIORITY 15
#define SSP_OPERATIONCOUNTER_PRIORITY 59
#define PACKAGE_FILESYSTEM_PRIORITY 61
#define PROFILER_PRIORITY 62
#define FUTURE_PRIORITY 63

namespace FlamingTorch
{
	typedef uint32 VersionType;

	/*!
	*	Core Utilities
	*/
	class FLAMING_API CoreUtils
	{
	public:
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
		*	\param b the fourth byte
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
	};
};
