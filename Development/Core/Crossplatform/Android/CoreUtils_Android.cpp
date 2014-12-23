#include "FlamingCore.hpp"
#if FLPLATFORM_ANDROID

namespace FlamingTorch
{
	std::string CoreUtils::PlatformString(bool IncludeVersion)
	{
		utsname Info;

		if(0 != uname(&Info))
			return "Android";

		return std::string(Info.sysname) + " " + Info.version + " " + Info.release;
	};

	int32 CoreUtils::RunProgram(const std::string &_ExePath, const std::string &Parameters, const std::string &WorkingDirectory)
	{
		return 0;
	};

	Rect CoreUtils::GetDesktopWorkArea()
	{
		return Rect();
	};
};

#endif
