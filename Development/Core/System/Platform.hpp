#pragma once
#if _WIN32 || _WIN64
#	if _WIN64
#		define FLPLATFORM_64BITS 1
#	else
#		define FLPLATFORM_64BITS 0
#	endif
#endif

// Check GCC
#if __GNUC__
#	if __x86_64__ || __ppc64__
#		define FLPLATFORM_64BITS 1
#	else
#		define FLPLATFORM_64BITS 0
#	endif
#endif

#if defined(__ANDROID__)
#	define FLPLATFORM_ANDROID 1
#	define FLPLATFORM_MOBILE 1
#else
#	define FLPLATFORM_ANDROID 0
#	define FLPLATFORM_MOBILE 0
#endif

#if defined(_WIN32) || defined(WIN32) || defined(WIN64) || defined(_WIN64)
#	if defined (_WIN32)
#		define FLPLATFORM_WIN32 1
#	endif
#
#	if defined (WIN32)
#		define FLPLATFORM_WIN64 1
#	endif
#
#	define FLPLATFORM_WINDOWS 1
#	define FLPLATFORM_PATHSPLIT '\\'
#	define FLPLATFORM_PATHSPLITSTRING "\\"
#
#	if _DEBUG
#		define DEBUG_BREAK __asm{int 3}
#	else
#		define DEBUG_BREAK
#	endif
#
#	define FLDLLEXPORT extern "C" __declspec(dllexport)
#
#elif defined(__linux__)
#	define FLPLATFORM_LINUX 1
#	define FLPLATFORM_PATHSPLIT '/'
#	define FLPLATFORM_PATHSPLITSTRING "/"
#	define DEBUG_BREAK __asm__("int $3\n" : : );
#	define FLDLLEXPORT extern "C"
#
#elif defined(__APPLE__)
#	define FLPLATFORM_MACOSX 1
#	define FLPLATFORM_PATHSPLIT '/'
#	define FLPLATFORM_PATHSPLITSTRING "/"
#	define DEBUG_BREAK __asm__("int $3\n" : : );
#	define FLDLLEXPORT extern "C"
#
#elif defined(EMSCRIPTEN)
#	define FLPLATFORM_EMSCRIPTEN 1
#	define FLPLATFORM_PATHSPLIT '/'
#	define FLPLATFORM_PATHSPLITSTRING "/"
#	define DEBUG_BREAK __asm__("int $3\n" : : );
#	define FLDLLEXPORT extern "C"
#else
#	error Unknown or unsupported platform
#endif

#define FLPLATFORM_UNIVERSAL_PATHSPLIT '/'
#define FLPLATFORM_UNIVERSAL_PATHSPLITSTRING "/"

#ifdef _DEBUG
#	define FLPLATFORM_DEBUG 1
#else
#	define FLPLATFORM_DEBUG 0
#endif

#if defined(_UNICODE) || defined(UNICODE)
#	define FLPLATFORM_UNICODE 1
#else
#	define FLPLATFORM_UNICODE 0
#endif

#ifdef __GNUC__
#	define FLAMINGCOMPILER_GCC
#	define FLAMINGCOMPILER_VERSION_MAJOR __GNUC__
#	define FLAMINGCOMPILER_VERSION_MINOR __GNUC_MINOR__
#	define FLAMINGCOMPILER_VERSION_BUILD __GNUC_PATCHLEVEL__
#elif defined(_MSC_VER)
#	define FLAMINGCOMPILER_MSVC
#	define FLAMINGCOMPILER_VERSION _MSC_VER
#	pragma warning(disable:4251)
#	pragma warning(disable:4273)
/*
#	pragma warning(disable:4313)
#	pragma warning(disable:4239)
#	pragma warning(disable:4201)
#	ifndef NO_DISABLE_POSSIBLE_BUG_DETECTING_WARNINGS
#		pragma warning(disable:4706)
#		pragma warning(disable:4100)
#		pragma warning(disable:4127)
#		pragma warning(disable:4512)
#	endif
*/
#else
#	error Unknown or unsupported compiler
#endif

namespace FlamingTorch
{
	void PrintStack();
}
