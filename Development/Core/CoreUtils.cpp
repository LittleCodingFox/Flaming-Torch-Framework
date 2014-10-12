#include "FlamingCore.hpp"
#if FLPLATFORM_WINDOWS
#	include <windows.h>
#endif

namespace FlamingTorch
{
	std::string CoreUtils::MakeVersionString(uint8 a, uint8 b)
	{
		std::stringstream str;
		str << (uint32)a << "." << (uint32)b;

		return str.str();
	};

	int32 CoreUtils::RunProgram(const std::string &_ExePath, const std::string &Parameters, const std::string &WorkingDirectory)
	{
		std::string ExePath = _ExePath;
#if FLPLATFORM_WINDOWS
		if(ExePath.rfind(".exe") != ExePath.length() - 4)
			ExePath += ".exe";

		ExePath = Path(ExePath).FullPath();

		Log::Instance.LogDebug("Core", "Running Program '%s' with arguments '%s' (WD: '%s')", ExePath.c_str(), Parameters.c_str(),
			WorkingDirectory.c_str());

		SHELLEXECUTEINFOA sei;
		memset(&sei, 0, sizeof(sei));
		sei.cbSize = sizeof(sei);
		sei.lpVerb = "open";
		sei.lpFile = ExePath.c_str();
		sei.lpParameters = Parameters.c_str();
		sei.lpDirectory = WorkingDirectory.c_str();
		sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
		sei.nShow = SW_HIDE;

		if(!ShellExecuteExA(&sei))
		{
			return -1;
		};

		DWORD ExitCode = 1;

		while(GetExitCodeProcess(sei.hProcess, &ExitCode) && ExitCode == STILL_ACTIVE);

		CloseHandle(sei.hProcess);

		return ExitCode;
#else

		Log::Instance.LogDebug("Core", "Running Program '%s' with arguments '%s' (WD: '%s')", ExePath.c_str(), Parameters.c_str(),
			WorkingDirectory.c_str());

		std::string CommandString = "\"" + ExePath + "\" " + Parameters;

		system(CommandString.c_str());

		return 0;
#endif
	};

	Rect CoreUtils::GetDesktopWorkArea()
	{
#if FLPLATFORM_WINDOWS
		MONITORINFOEXA MonitorInfo;
		memset(&MonitorInfo, 0, sizeof(MonitorInfo));

		MonitorInfo.cbSize = sizeof(MONITORINFOEXA);

		HMONITOR Monitor = MonitorFromWindow(GetActiveWindow(), MONITOR_DEFAULTTOPRIMARY);

		if(Monitor == INVALID_HANDLE_VALUE || !GetMonitorInfoA(Monitor, &MonitorInfo))
			return Rect();

		return Rect((f32)MonitorInfo.rcWork.left, (f32)MonitorInfo.rcWork.right, (f32)MonitorInfo.rcWork.top, (f32)MonitorInfo.rcWork.bottom);
#else
		return Rect();
#endif
	};
};
