#include "FlamingCore.hpp"
#if FLPLATFORM_WINDOWS
#	include <windows.h>
#	include "dbghelp.h"
#	pragma comment(lib, "dbghelp.lib")
#elif !FLPLATFORM_ANDROID && !FLPLATFORM_EMSCRIPTEN
#	include <execinfo.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
namespace FlamingTorch
{
#if FLPLATFORM_MOBILE
#	define PLATFORMTYPE_DEFAULT PlatformType::Mobile;
#else
#	define PLATFORMTYPE_DEFAULT PlatformType::PC;
#endif

	uint8 PlatformInfo::PlatformType = PLATFORMTYPE_DEFAULT;

	uint8 PlatformInfo::ScreenRotation = ScreenRotation::North;

	uint32 PlatformInfo::ResolutionOverrideWidth = 0;
	uint32 PlatformInfo::ResolutionOverrideHeight = 0;

	Rect PlatformInfo::RotateScreen(const Rect &In)
	{
		Rect Temp = ToNorthRotation(In);
		Rect Out = Temp;

		switch (ScreenRotation)
		{
		case ScreenRotation::South:
			Out = Rect(Temp.Left, Temp.Right, Temp.Bottom, Temp.Top);

			break;

		case ScreenRotation::East:
			Out = Rect(Temp.Top, Temp.Bottom, Temp.Left, Temp.Right);

			break;

		case ScreenRotation::West:
			Out = Rect(Temp.Top, Temp.Bottom, Temp.Right, Temp.Left);

			break;

		default:
			return Temp;
		}

		return Out;
	}

	uint8 PlatformInfo::ScreenOrientation(const Rect &In)
	{
		if (PlatformType == PlatformType::Mobile)
		{
			if (In.Bottom > In.Right)
				return ScreenRotation::North;

			if (In.Top > In.Right)
				return ScreenRotation::South;

			if (In.Bottom < In.Right)
				return ScreenRotation::West;

			if (In.Bottom < In.Left)
				return ScreenRotation::East;
		}
		else
		{
			if (In.Right > In.Bottom)
				return ScreenRotation::North;

			if (In.Right > In.Top)
				return ScreenRotation::South;

			if (In.Right < In.Bottom)
				return ScreenRotation::West;

			if (In.Left < In.Bottom)
				return ScreenRotation::East;
		}

		return 0xFF;
	}

	f32 PlatformInfo::ScreenRotationValue()
	{
		f32 Degrees = 0;

		if (PlatformType == PlatformType::Mobile)
		{
			Degrees = -90;
		}

		switch (ScreenRotation)
		{
		case ScreenRotation::South:
			Degrees += 180;

			break;

		case ScreenRotation::East:
			Degrees += 90;

			break;

		case ScreenRotation::West:
			Degrees += 270;

			break;
		}

		return MathUtils::DegToRad(Degrees);
	}

	Rect PlatformInfo::ToNorthRotation(const Rect &In)
	{
		f32 Max = In.Left, PrevMax = In.Left;

		if (In.Right > Max)
		{
			PrevMax = Max;
			Max = In.Right;
		}
		else if (In.Right > PrevMax)
		{
			PrevMax = In.Right;
		}

		if (In.Top > Max)
		{
			PrevMax = Max;
			Max = In.Top;
		}
		else if (In.Top > PrevMax)
		{
			PrevMax = In.Top;
		}

		if (In.Bottom > Max)
		{
			PrevMax = Max;
			Max = In.Bottom;
		}
		else if (In.Bottom > PrevMax)
		{
			PrevMax = In.Bottom;
		}

		Rect Out = PlatformType == PlatformType::Mobile ? Rect(0, PrevMax, 0, Max) : Rect(0, Max, 0, PrevMax);

		return Out;
	}

	Vector2 PlatformInfo::ScreenSize(const Rect &In)
	{
		Vector2 Out(In.Left > In.Right ? In.Left : In.Right, In.Top > In.Bottom ? In.Top : In.Bottom);

		return Out;
	}

#if FLPLATFORM_WINDOWS
	void PrintStack()
	{
		void *Stack[1024];
		uint16 Frames;
		HANDLE Process = GetCurrentProcess();

		SymInitialize(Process, ".", TRUE);
		SymSetOptions(SYMOPT_LOAD_LINES);

		Frames = CaptureStackBackTrace( 0, 100, Stack, NULL );
		DWORD Displacement = 0;
		IMAGEHLP_LINE64 Line;
		Line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

		Log::Instance.LogInfo("Core", "Dumping %d stack frames", Frames - 2);

		//Since this is called from FLASSERTs only, there are always two extra frames, so we can just ignore them
		for(uint16 i = 0; i < Frames - 2; i++ )
		{
			SymGetLineFromAddr64(Process, (DWORD64)Stack[i], &Displacement, &Line);

			Log::Instance.LogInfo("Core", "%d: %s:%d", Frames - i - 3, Line.FileName, Line.LineNumber);
		}

		SymCleanup(Process);
	}
#elif !FLPLATFORM_ANDROID && !FLPLATFORM_EMSCRIPTEN
	void PrintStack()
	{
		void* Stack[1024];
		uint16 Frames = backtrace(Stack, 1024);
		char** Strings = backtrace_symbols(Stack, Frames);
		
		Log::Instance.LogInfo("Core", "Dumping %d stack frames", Frames - 2);
		
		//Since this is called from FLASSERTs only, there are always two extra frames, so we can just ignore them
		for(uint16 i = 0; i < Frames - 2; i++ )
		{
			Log::Instance.LogInfo("Core", "%d: %s", Frames - i - 3, Strings[i]);
		}
		
		free(Strings);
	}
#else
	void PrintStack()
	{
		Log::Instance.LogInfo("Core", "Stack Unavailable");
	}
#endif

	void LibWarning(unsigned int Line, const char* FileName,
		const char* Function, const char* Variable, const char *Message, ...)
	{
		static char Buffer[1024];
		va_list args;
		va_start(args, Message);
		vsnprintf(Buffer, 1024, Message, args);
		va_end(args);

		Log::Instance.LogErr("Core", "Assertion failed on '%s' (FN: '%s'; F: '%s:%d' R: '%s').", Variable, Function, FileName, Line, Buffer);

#if !FLPLATFORM_ANDROID && !FLPLATFORM_EMSCRIPTEN
		PrintStack();
#endif

#if FLPLATFORM_DEBUG
		DEBUG_BREAK;
#endif
	}
}
