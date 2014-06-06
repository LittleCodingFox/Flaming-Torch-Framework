#pragma once

namespace FlamingTorch
{
#if FLPLATFORM_WINDOWS
	/*!
	*	Enables Minidumps
	*	\param AppName the name of the application
	*	\param VersionString the application's Version as a String
	*/
	void FLAMING_API EnableMinidumps(const char *AppName, const char *VersionString);
#endif
};
