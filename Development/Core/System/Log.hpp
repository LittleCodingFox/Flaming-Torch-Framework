#pragma once

/*!
*	Logging class
*/
class Log : public SubSystem
{
private:
	FILE *LogFile;

	std::vector<std::string> HiddenTags;

	void BaseLog(uint8 Level, const std::string &Message);
public:
	/*!
	*	Whether to print time to the console
	*	Only change this after InitSubsystems was called
	*/
	bool PrintTime;

	/*!
	*	Folder Name Override
	*	Used to override the default folder name
	*	Leave empty to use the normal log folder name
	*/
	std::string FolderName;

	Log() : SubSystem(LOG_PRIORITY), LogFile(NULL), PrintTime(false) {}

	/*!
	*	Hides a Tag from being logged
	*/
	void HideTag(const std::string &Tag);

	/*!
	*	Log an Information event
	*	\param Tag the event's Tag
	*	\param Format the event's Format (same as Printf)
	*/
	void LogInfo(const char *Tag, const char *Format, ...);
	/*!
	*	Log a Warning event
	*	\param Tag the event's Tag
	*	\param Format the event's Format (same as Printf)
	*/
	void LogWarn(const char *Tag, const char *Format, ...);
	/*!
	*	Log an Error event
	*	\param Tag the event's Tag
	*	\param Format the event's Format (same as Printf)
	*/
	void LogErr(const char *Tag, const char *Format, ...);
	/*!
	*	Log a Debug event
	*	\param Tag the event's Tag
	*	\param Format the event's Format (same as Printf)
	*/
	void LogDebug(const char *Tag, const char *Format, ...);

	void StartUp(uint32 Priority);
	void Shutdown(uint32 Priority);
};

extern Log g_Log;
