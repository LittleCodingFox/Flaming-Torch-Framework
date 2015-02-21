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
	std::string CoreUtils::MakeVersionString(uint8 a, uint8 b)
	{
		std::stringstream str;
		str << (uint32)a << "." << (uint32)b;

		return str.str();
	}

#if FLPLATFORM_WINDOWS
	void PrintStack()
	{
		void *Stack[1024];
		uint16 Frames;
		HANDLE Process = GetCurrentProcess();

		SymInitialize(Process, ".", TRUE);
		SymSetOptions(SYMOPT_LOAD_LINES);

		Frames = CaptureStackBackTrace(0, 100, Stack, NULL);
		DWORD Displacement = 0;
		IMAGEHLP_LINE64 Line;
		Line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

		g_Log.LogInfo("Core", "Dumping %d stack frames", Frames - 2);

		//Since this is called from FLASSERTs only, there are always two extra frames, so we can just ignore them
		for (uint16 i = 0; i < Frames - 2; i++)
		{
			SymGetLineFromAddr64(Process, (DWORD64)Stack[i], &Displacement, &Line);

			g_Log.LogInfo("Core", "%d: %s:%d", Frames - i - 3, Line.FileName, Line.LineNumber);
		}

		SymCleanup(Process);
	}
#elif !FLPLATFORM_ANDROID && !FLPLATFORM_EMSCRIPTEN
	void PrintStack()
	{
		void* Stack[1024];
		uint16 Frames = backtrace(Stack, 1024);
		char** Strings = backtrace_symbols(Stack, Frames);

		g_Log.LogInfo("Core", "Dumping %d stack frames", Frames - 2);

		//Since this is called from FLASSERTs only, there are always two extra frames, so we can just ignore them
		for (uint16 i = 0; i < Frames - 2; i++)
		{
			g_Log.LogInfo("Core", "%d: %s", Frames - i - 3, Strings[i]);
		}

		free(Strings);
	}
#else
	void PrintStack()
	{
		g_Log.LogInfo("Core", "Stack Unavailable");
	}
#endif

	void CoreUtils::Assert(unsigned int Line, const char* FileName,
		const char* Function, const char* Variable, const char *Message, ...)
	{
		static char Buffer[1024];
		va_list args;
		va_start(args, Message);
		vsnprintf(Buffer, 1024, Message, args);
		va_end(args);

		g_Log.LogErr("Core", "Assertion failed on '%s' (FN: '%s'; F: '%s:%d' R: '%s').", Variable, Function, FileName, Line, Buffer);

#if !FLPLATFORM_ANDROID && !FLPLATFORM_EMSCRIPTEN
		PrintStack();
#endif

#if FLPLATFORM_DEBUG
		DEBUG_BREAK;
#endif
	}
}
