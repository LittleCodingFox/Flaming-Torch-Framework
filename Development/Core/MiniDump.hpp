#pragma once

#if FLPLATFORM_WINDOWS
	/*!
	*	Enables Minidumps
	*	\param AppName the name of the application
	*	\param VersionString the application's Version as a String
	*/
	void EnableMinidumps(const char *AppName, const char *VersionString);
#endif
