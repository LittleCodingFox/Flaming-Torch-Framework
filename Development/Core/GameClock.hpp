#pragma once
class FLAMING_API GameClock : public SubSystem
{
private:
	f32 ActualDeltaTime;
	uint64 AccountedTime, LastTimeFrame, AccumulatedTime, LastAccumulationTimeFrame;
	bool Paused;
	uint32 FixedStepFrameRate, FixedStepInterval;
	f32 FixedStepFrameDelta;

	void UpdateDelta();

	GameClock() : Paused(false), ActualDeltaTime(0), AccountedTime(0), AccumulatedTime(0), LastTimeFrame(0),
		LastAccumulationTimeFrame(0), SubSystem(GAMECLOCK_PRIORITY)
	{
		SetFixedStepRate(30);
	};
public:
	static GameClock Instance;

	~GameClock() {};
	void StartUp(uint32 Priority);
	void Shutdown(uint32 Priority);
	void Update(uint32 Priority);

	void Pause();
	void Unpause();
	uint64 GetElapsedTime();
	float Delta();
	std::string CurrentTimeAsString();
	uint64 CurrentTime();
	void SetFixedStepRate(uint32 FPS);
	uint64 GetFixedStepInterval();
	uint32 GetFixedStepRate();
	//Checks AccumulatedTime and subtracts the Fixed Step Interval should it pass.
	//Call this while its true to run multiple Fixed Step frames
	bool MayPerformFixedStepStep();
	f32 FixedStepDelta();
};

//Current game time
uint64 FLAMING_API GameClockTime();
//Current game time (no pauses applied)
uint64 FLAMING_API GameClockTimeNoPause();

//Game time difference
/*
	Used like this:
	Where you used to do <custom timer>.GetElapsedTime()
	you do GameClockDiff(<starting time>)
	where the starting time was set once with GameClockTime
*/
uint64 FLAMING_API GameClockDiff(uint64 offset);
uint64 FLAMING_API GameClockDiffNoPause(uint64 offset);
f32 FLAMING_API GameClockDelta();
void FLAMING_API GameClockSetFixedFrameRate(uint32 FPS);
bool FLAMING_API GameClockMayPerformFixedTimeStep();
//Fixed-step delta
f32 FLAMING_API GameClockFixedDelta();
