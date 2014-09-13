#pragma once
/*!
*	Linear Timer
*	A timer that calculates a value between 0 and 1 as time progresses, then returns to the original value, repeating this
*/
class LinearTimer
{
private:
	uint64 StartTime;
	bool GoingUp;
public:
	/*!
	*	Time Interval
	*/
	uint64 Interval;

	LinearTimer() : Interval(0), StartTime(0), GoingUp(true) {};

	/*!
	*	Current Linear Value
	*/
	f32 Value();

	/*!
	*	Current Linear Value
	*	\note Ignores clockpauses
	*/
	f32 ValueNoPause();
	
	/*!
	*	Resets this LinearTimer
	*/
	void Reset();
};

/*!
*	Game Clock
*/
class GameClock : public SubSystem
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

	/*!
	*	Pauses the game clock
	*/
	void Pause();

	/*!
	*	Unpauses the game clock
	*/
	void Unpause();
	
	/*!
	*	Gets the time since we started running
	*/
	uint64 GetElapsedTime();
	
	/*!
	*	Gets the current frame delta time
	*/
	f32 Delta();
	
	/*!
	*	Gets the current time as a string
	*/
	std::string CurrentTimeAsString();
	
	/*!
	*	Gets the current time
	*/
	uint64 CurrentTime();

	/*!
	*	Gets the current date and time as a string
	*/
	std::string CurrentDateTimeAsString();
	
	/*!
	*	Sets our fixed step Framerate
	*	\param FPS the target FPS
	*/
	void SetFixedStepRate(uint32 FPS);

	/*!
	*	Gets the fixed step interval
	*/
	uint64 GetFixedStepInterval();

	/*!
	*	Gets the fixed step rate (FPS)
	*/
	uint32 GetFixedStepRate();
	
	/*!
	*	Whether we may perform a Fixed Step Step
	*/
	bool MayPerformFixedStepStep();
	
	/*!
	*	The Fixed Step Delta
	*/
	f32 FixedStepDelta();
};

/*!
*	Current Time
*/
uint64 GameClockTime();

/*!
*	Current Time
*	\note ignores pauses
*/
uint64 GameClockTimeNoPause();

/*!
*	Get the time difference with another time offset
*	\param offset the other time offset
*/
uint64 GameClockDiff(uint64 offset);

/*!
*	Get the time difference with another time offset
*	\param offset the other time offset
*	\note ignores pauses
*/
uint64 GameClockDiffNoPause(uint64 offset);

/*!
*	Get the delta time since the last frame
*/
f32 GameClockDelta();

/*!
*	Sets the Fixed Frame Rate of the game clock
*	\param FPS the target FPS
*/
void GameClockSetFixedFrameRate(uint32 FPS);

/*!
*	Check whether we may perform a fixed time step
*/
bool GameClockMayPerformFixedTimeStep();

/*!
*	Get the fixed time step delta
*/
f32 GameClockFixedDelta();
