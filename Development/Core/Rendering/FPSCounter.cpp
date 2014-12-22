#include "FlamingCore.hpp"

namespace FlamingTorch
{
	FPSCounter FPSCounter::Instance;

	uint32 FPSCounter::FPS()
	{
		return CurrentFPS;
	};

	void FPSCounter::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK()

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo("FPSCounter", "Initializing FPS Counter...");
	};

	void FPSCounter::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		SubSystem::Shutdown(Priority);

		Log::Instance.LogInfo("FPSCounter", "Terminating FPS Counter...");
	};

	void FPSCounter::Update(uint32 Priority)
	{
		SubSystem::Update(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Time += GameClock::Instance.Delta();
		Counter++;

		if(Time >= 1.f)
		{
			Time -= 1.f;
			CurrentFPS = Counter;
			Counter = 0;
		};
	};
};
