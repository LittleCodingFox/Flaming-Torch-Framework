#include "FlamingCore.hpp"
namespace FlamingTorch
{
#if PROFILER_ENABLED
	Profiler Profiler::Instance;

	void Profiler::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK()

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		UpdateTimer = GameClockTimeNoPause();

		Log::Instance.LogInfo("Profiler", "Starting Profiler Subsystem");
	};

	void Profiler::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		SubSystem::Shutdown(Priority);

		Log::Instance.LogInfo("Profiler", "Terminating Profiler Subsystem");

		WasStarted = false;

		std::stringstream str;

		for(PacketMap::iterator it = Packets.begin(); it != Packets.end(); it++)
		{
			uint32 Average = (uint32)it->second.ms / it->second.Count;

			str.str("");
			str << "[" << Profiler::StatTypeString(it->second.Type) << "] " << it->first << " " << it->second.ms << " (" << Average << " avg)";

			Log::Instance.LogInfo("Profiler", str.str().c_str());
		};
	};

	void Profiler::Update(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

#if !ANDROID
		sf::Lock lock(Lock);
#endif

		if(GameClockDiffNoPause(UpdateTimer) >= 1000)
		{
			UpdateTimer += 1000;

			OnFinishFrame(Packets);

			for(PacketMap::iterator it = Packets.begin(); it != Packets.end(); it++)
			{
				it->second.FrameMS = 0;
			};
		};
	};

	void Profiler::ReportStat(const std::string &Name, uint32 Type, uint64 ms)
	{
#if !ANDROID
		sf::Lock lock(Lock);
#endif

		PacketMap::iterator it = Packets.find(Name);

		if(it != Packets.end())
		{
			it->second.Count++;
			it->second.ms += ms;
			it->second.FrameMS += ms;
		}
		else
		{
			Packet p;
			p.Count = 1;
			p.ms = ms;
			p.FrameMS = ms;
			p.Type = Type;
			Packets[Name] = p;
		};
	};

	std::string Profiler::StatTypeString(uint32 Type)
	{
		switch(Type)
		{
		case StatTypes::Game:
			return "Game";

			break;
		case StatTypes::Networking:
			return "Networking";

			break;
		case StatTypes::Rendering:
			return "Rendering";

			break;
		case StatTypes::Sound:
			return "Sound";

			break;
		};

		return "UNKNOWN";
	};

	ProfilerFragment::~ProfilerFragment()
	{
		uint64 CurrentTime = GameClock::Instance.CurrentTime();

		uint64 ms = CurrentTime - Start;

		Profiler::Instance.ReportStat(NameString, FragmentType, ms);
	};

	void ProfilerFragment::ReportPercentage(uint32 Percentage)
	{
		uint64 CurrentTime = GameClock::Instance.CurrentTime();

		uint64 ms = CurrentTime - Start;

		static std::stringstream str;

		str.str("");
		str << NameString << " @ " << Percentage;
		Profiler::Instance.ReportStat(str.str(), FragmentType, ms);
	};
#endif
};
