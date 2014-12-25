#include "FlamingCore.hpp"
#if FLPLATFORM_WINDOWS
#	include <windows.h>
#	include "dbghelp.h"
#	pragma comment(lib, "dbghelp.lib")
#elif !ANDROID
#	include <execinfo.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
namespace FlamingTorch
{
	PlatformInfo PlatformInfo::Instance;

	PlatformInfo::PlatformInfo() : ResolutionOverrideWidth(0), ResolutionOverrideHeight(0)
	{
#if FLPLATFORM_MOBILE
		PlatformType = PlatformType::Mobile;
#else
		PlatformType = PlatformType::PC;
#endif
	};

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
		};

		SymCleanup(Process);
	};
#elif !ANDROID
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
		};
		
		free(Strings);
	};
#else
	void PrintStack()
	{
		Log::Instance.LogInfo("Core", "Stack Unavailable");
	};
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

#if !ANDROID
		PrintStack();
#endif

#if FLPLATFORM_DEBUG
		DEBUG_BREAK;
#endif
	};
};
