#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	define TAG "Future"

	Future g_Future;

	void Future::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK();

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		g_Log.LogInfo(TAG, "Starting Future Subsystem");
	}

	void Future::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();
		WasStarted = false;

		g_Log.LogInfo(TAG, "Shutting down Future Subsystem");
	}

	void Future::Update(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		bool Found = true;

		while(Found)
		{
			Found = false;

			for(std::list<DisposablePointer<FutureInfo> >::reverse_iterator rit = Futures.rbegin(); rit != Futures.rend(); rit++)
			{
				if(GameClockDiff(rit->Get()->StartTime) >= rit->Get()->Length)
				{
					std::list<DisposablePointer<FutureInfo> >::iterator it = --rit.base();

					rit->Get()->Signal(rit->Get()->Stream);

					Futures.erase(it);

					Found = true;

					break;
				}
			}
		}
	}

	void Future::Post(FutureFn Ptr, const MemoryStream &Stream)
	{
		if(!WasStarted)
		{
			g_Log.LogErr("Future", "Future Subsystem not started yet!");

			return;
		}

		DisposablePointer<FutureInfo> Future(new FutureInfo());
		Future->StartTime = GameClockTimeNoPause();
		Future->Length = 0;
		Future->Stream = Stream;
		Future->Stream.Seek(0);
		Future->Signal.Connect(Ptr);

		Futures.push_back(Future);

		return;
	}

	void Future::PostDelayed(FutureFn Ptr, uint32 Length, const MemoryStream &Stream)
	{
		if(!WasStarted)
		{
			g_Log.LogErr("Future", "Future Subsystem not started yet!");

			return;
		}

		DisposablePointer<FutureInfo> Future(new FutureInfo());
		Future->StartTime = GameClockTimeNoPause();
		Future->Length = Length;
		Future->Stream = Stream;
		Future->Stream.Seek(0);
		Future->Signal.Connect(Ptr);

		Futures.push_back(Future);
	}
}
