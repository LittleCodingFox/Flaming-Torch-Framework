#pragma once
namespace StatTypes
{
	enum
	{
		Game = 0,
		Rendering,
		Networking,
		Sound,
		Count
	};
};

class Profiler : public SubSystem
{
public:
	struct Packet
	{
		uint32 Type, Count;
		uint64 ms, FrameMS;
	};

	typedef std::map<std::string, Packet> PacketMap;
	PacketMap Packets;

	static const uint32 MinFPS = 60;
	static const uint32 BaseTimeout = 1000 / MinFPS;
	uint64 UpdateTimer;

	sf::Mutex Lock;

private:
	Profiler() : SubSystem(PROFILER_PRIORITY) {};
public:
	static Profiler Instance;

	~Profiler() {};

	void StartUp(uint32 Priority);
	void Shutdown(uint32 Priority);
	void Update(uint32 Priority);

	void ReportStat(const std::string &Name, uint32 Type, uint64 ms);
	static std::string StatTypeString(uint32 Type);

	Signal1<const PacketMap &> OnFinishFrame;
};

class ProfilerFragment
{
private:
	uint64 Start;
	std::string NameString;
	uint32 FragmentType;
public:
	ProfilerFragment(const char *Name, uint32 Type) : Start(GameClock::Instance.CurrentTime()),
		NameString(Name), FragmentType(Type) {};
	~ProfilerFragment();

	void ReportPercentage(uint32 Percentage);
};

#if PROFILER_ENABLED
#	define PROFILE(Name, Type) ProfilerFragment __PROFILER__(Name, Type);
#	define PROFILE_PROGRESS(Percentage) __PROFILER__.ReportPercentage(Percentage);
#else
#	define PROFILE(Name, Type)
#	define PROFILE_PROGRESS(Percentage)
#endif
