#include "FlamingCore.hpp"
namespace FlamingTorch
{
	Log Log::Instance;

	namespace LoggingLevel
	{
		enum
		{
			Info,
			Warn,
			Err,
			Debug
		};
	};

	inline std::string LogTime(bool FileName = false)
	{
		time_t rawtime;
		time(&rawtime);

		static char Buffer[128];

		strftime(Buffer, 128, FileName ? "%d_%m_%Y" : "%d/%m/%Y %H:%M:%S",
			localtime(&rawtime));

		return Buffer;
	};

	void Log::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK()

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		PrintTime = false;

		DirectoryInfo::CreateDirectory(std::string(DirectoryInfo::PreferredStorageDirectory()));
		DirectoryInfo::CreateDirectory(std::string(DirectoryInfo::PreferredStorageDirectory()) + "/Logs");

		std::string FileName = std::string(DirectoryInfo::PreferredStorageDirectory()) + std::string("/Logs/") +
			LogTime(true) + ".txt";

		LogFile = fopen(FileName.c_str(), "a+");

		if(LogFile == NULL)
		{
			LogErr("Log", "Log File is unavailable!");
		}
		else
		{
			fputs("\n\n>>>>>> Starting Session <<<<<<\n", LogFile);
			fprintf(LogFile, "Core Build: %s\n\n", CoreUtils::MakeVersionString(FTSTD_VERSION_MAJOR, FTSTD_VERSION_MINOR).c_str());
		};

		LogInfo("Log", "Starting Logging Subsystem");
	};

	void Log::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		LogInfo("Log", "Terminating Logging Subsystem");

		SubSystem::Shutdown(Priority);
	};

	void Log::BaseLog(uint8 Level, const std::string &Message)
	{
		if(!WasStarted)
			return;

		std::string Prefix;

		switch(Level)
		{
		case LoggingLevel::Info:
			Prefix = "[Info] ";

			break;
		case LoggingLevel::Warn:
			Prefix = "[Warn] ";

			break;
		case LoggingLevel::Err:
			Prefix = "[Error] ";

			break;
		case LoggingLevel::Debug:
			Prefix = "[Debug] ";

			break;
		};

		std::string Final = Prefix + LogTime() + std::string(": ") + Message + "\n";
		std::string FinalPrint = Prefix + std::string(": ") + Message + "\n";

		if(LogFile)
		{
			fputs(Final.c_str(), LogFile);
			fflush(LogFile);
		};

		printf("%s", (PrintTime ? Final : FinalPrint).c_str());

#if ANDROID
		__android_log_print(ANDROID_LOG_DEBUG, "Core", (PrintTime ? Final : FinalPrint).c_str());
#endif
	};

	inline std::string ProcessMessage(const char *Format, va_list vp)
	{
		static char Buffer[102400];
		vsnprintf(Buffer, 102400, Format, vp);

		return Buffer;
	};

	void Log::HideTag(const std::string &Tag)
	{
		HiddenTags.push_back(Tag);
	};

	void Log::LogInfo(const char *Tag, const char *Format, ...)
	{
		if(std::find(HiddenTags.begin(), HiddenTags.end(), Tag) != HiddenTags.end())
			return;

		va_list vp;
		va_start(vp, Format);
		std::stringstream str;
		str << "[" << Tag << "] " << ProcessMessage(Format, vp);
		va_end(vp);

		BaseLog(LoggingLevel::Info, str.str());
	};

	void Log::LogWarn(const char *Tag, const char *Format, ...)
	{
		if(std::find(HiddenTags.begin(), HiddenTags.end(), Tag) != HiddenTags.end())
			return;

		va_list vp;
		va_start(vp, Format);
		std::stringstream str;
		str << "[" << Tag << "] " << ProcessMessage(Format, vp);
		va_end(vp);

		BaseLog(LoggingLevel::Warn, str.str());
	};

	void Log::LogErr(const char *Tag, const char *Format, ...)
	{
		if(std::find(HiddenTags.begin(), HiddenTags.end(), Tag) != HiddenTags.end())
			return;

		va_list vp;
		va_start(vp, Format);
		std::stringstream str;
		str << "[" << Tag << "] " << ProcessMessage(Format, vp);
		va_end(vp);

		BaseLog(LoggingLevel::Err, str.str());
	};

	void Log::LogDebug(const char *Tag, const char *Format, ...)
	{
#if FLPLATFORM_DEBUG
		if(std::find(HiddenTags.begin(), HiddenTags.end(), Tag) != HiddenTags.end())
			return;

		va_list vp;
		va_start(vp, Format);
		std::stringstream str;
		str << "[" << Tag << "] " << ProcessMessage(Format, vp);
		va_end(vp);

		BaseLog(LoggingLevel::Debug, str.str());
#endif
	};
};
