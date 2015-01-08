#include "FlamingCore.hpp"
#if FLPLATFORM_WINDOWS
#	include <Windows.h>
#	include <Dbghelp.h>
#	include <StrSafe.h>
namespace FlamingTorch
{
	std::string Name, Version;

	LONG WINAPI GenerateDump(EXCEPTION_POINTERS* pExceptionPointers)
	{
		BOOL bMiniDumpSuccessful;
		CHAR szFileName[MAX_PATH]; 
		CHAR* szAppName = (CHAR *)Name.c_str();
		CHAR* szVersion = (CHAR *)Version.c_str();
		DWORD dwBufferSize = MAX_PATH;
		HANDLE hDumpFile;
		SYSTEMTIME stLocalTime;
		MINIDUMP_EXCEPTION_INFORMATION ExpParam;

		GetLocalTime(&stLocalTime);

		StringCchPrintfA(szFileName, MAX_PATH, "%s_%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp", 
			szAppName, szVersion, stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay, 
			stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond, GetCurrentProcessId(), GetCurrentThreadId());
		hDumpFile = CreateFileA(szFileName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

		ExpParam.ThreadId = GetCurrentThreadId();
		ExpParam.ExceptionPointers = pExceptionPointers;
		ExpParam.ClientPointers = TRUE;

		bMiniDumpSuccessful = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), 
			hDumpFile, MiniDumpWithDataSegs, &ExpParam, NULL, NULL);

		return EXCEPTION_EXECUTE_HANDLER;
	}

	void EnableMinidumps(const char *AppName, const char *VersionString)
	{
		Name = AppName;
		Version = VersionString;

		SetUnhandledExceptionFilter(GenerateDump);
	}
}

#endif
