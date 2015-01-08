#include "FlamingCore.hpp"
#if FLPLATFORM_LINUX
#include <sys/utsname.h>

namespace FlamingTorch
{
	std::string CoreUtils::PlatformString(bool IncludeVersion)
	{
		utsname Info;

		if(0 != uname(&Info))
			return "Linux";
		
		return std::string(Info.sysname) + " " + Info.version + " " + Info.release;
	}

	int32 CoreUtils::RunProgram(const std::string &_ExePath, const std::string &Parameters, const std::string &WorkingDirectory)
	{
		std::string ExePath = _ExePath;

		Log::Instance.LogDebug("Core", "Running Program '%s' with arguments '%s' (WD: '%s')", ExePath.c_str(), Parameters.c_str(),
			WorkingDirectory.c_str());

		std::string CommandString = "\"" + ExePath + "\" " + Parameters;

		system(CommandString.c_str());

		return 0;
	}

	Rect CoreUtils::GetDesktopWorkArea()
	{
	}
}

#endif