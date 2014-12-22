#include "FlamingCore.hpp"
#include <time.h>

namespace FlamingTorch
{
	GameClock GameClock::Instance;

	void GameClock::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK()

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo("GameClock", "Starting GameClock Subsystem");

		LastTimeFrame = CurrentTime();
	};

	void GameClock::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo("GameClock", "Terminating GameClock Subsystem");

		SubSystem::Shutdown(Priority);
	};

	void GameClock::Update(uint32 Priority)
	{
		if(Priority != GAMECLOCK_PRIORITY)
			return;

		SUBSYSTEM_PRIORITY_CHECK();

		UpdateDelta();
	};

	void GameClock::SetFixedStepRate(uint32 FPS)
	{
		FixedStepFrameRate = FPS;
		FixedStepInterval = 1000 / FPS;
		FixedStepFrameDelta = FixedStepInterval / 1000.f;
	};

	uint64 GameClock::GetFixedStepInterval()
	{
		return FixedStepInterval;
	};

	uint32 GameClock::GetFixedStepRate()
	{
		return FixedStepFrameRate;
	};

	bool GameClock::MayPerformFixedStepStep()
	{
		if(AccumulatedTime > 250)
			AccumulatedTime = 250;

		if(AccumulatedTime > FixedStepInterval)
		{
			AccumulatedTime -= FixedStepInterval;

			if(AccumulatedTime < FixedStepInterval)
				LastAccumulationTimeFrame = CurrentTime();

			return true;
		};

		return false;
	};

	f32 GameClock::FixedStepDelta()
	{
		return FixedStepFrameDelta;
	};

	void GameClock::Pause()
	{
		FLASSERT(!Paused, "Attempting to pause the clock when already paused!");
		AccountedTime = CurrentTime() - AccountedTime;
		Paused = true;
	};

	void GameClock::Unpause()
	{
		FLASSERT(Paused, "Attempting to unpause the clock when not paused!");
		Paused = false;
	};

	uint64 GameClock::GetElapsedTime()
	{
		return (Paused ? AccountedTime : CurrentTime() - AccountedTime);
	};

	void GameClock::UpdateDelta()
	{
		uint64 CurrentTimeFrame = CurrentTime();
		f32 t = MathUtils::Clamp((CurrentTimeFrame - LastTimeFrame) / 1000.f);

		ActualDeltaTime = t;
		AccumulatedTime += (CurrentTimeFrame - LastAccumulationTimeFrame);
		LastTimeFrame = CurrentTime();
		LastAccumulationTimeFrame = CurrentTime();
	};

	f32 GameClock::Delta()
	{
		return ActualDeltaTime;
	};

	std::string GameClock::CurrentTimeAsString()
	{
		char Buffer[100];
		sprintf(Buffer, "%lld", CurrentTime());

		return Buffer;
	};

	std::string GameClock::CurrentDateTimeAsString()
	{
		time_t rawtime;
		time(&rawtime);

		static char Buffer[128];

		strftime(Buffer, 128, "%d/%m/%Y %H:%M:%S", localtime(&rawtime));

		return Buffer;
	};

	uint64 GameClock::CurrentTime()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	};

	uint64 GameClockTime()
	{
		return GameClock::Instance.GetElapsedTime();
	};

	uint64 GameClockTimeNoPause()
	{
		return GameClock::Instance.CurrentTime();
	};

	uint64 GameClockDiff(uint64 offset)
	{
		uint64 Time = GameClockTime();

		return Time - offset;
	};

	uint64 GameClockDiffNoPause(uint64 offset)
	{
		return GameClockTimeNoPause() - offset;
	};

	f32 GameClockDelta()
	{
		return GameClock::Instance.Delta();
	};

	void GameClockSetFixedFrameRate(uint32 FPS)
	{
		GameClock::Instance.SetFixedStepRate(FPS);
	};

	bool GameClockMayPerformFixedTimeStep()
	{
		return GameClock::Instance.MayPerformFixedStepStep();
	};

	f32 GameClockFixedDelta()
	{
		return GameClock::Instance.FixedStepDelta();
	};

	f32 LinearTimer::Value()
	{
		if(Interval == 0)
			return 0;

		f32 Result = GameClockDiff(StartTime) / (f32)Interval;
		f32 ActualResult = MathUtils::Clamp(GoingUp ? Result : 1 - Result, 0, 1);

		if(Result > 1)
		{
			GoingUp = !GoingUp;
			StartTime = GameClockTime();
		};

		return ActualResult;
	};

	f32 LinearTimer::ValueNoPause()
	{
		if(Interval == 0)
			return 0;

		f32 Result = GameClockDiffNoPause(StartTime) / (f32)Interval;
		f32 ActualResult = MathUtils::Clamp(GoingUp ? Result : 1 - Result, 0, 1);

		if(Result > 1)
		{
			GoingUp = !GoingUp;
			StartTime = GameClockTime();
		};

		return ActualResult;
	};

	void LinearTimer::Reset()
	{
		StartTime = GameClockTime();
	};
};
