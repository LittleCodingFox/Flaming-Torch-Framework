#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	define TAG "Future"

	Future Future::Instance;

	void Future::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK()

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo(TAG, "Starting Future Subsystem");
	}

	void Future::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();
		WasStarted = false;

		Log::Instance.LogInfo(TAG, "Shutting down Future Subsystem");
	}

	void Future::Update(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		bool Found = true;

		while(Found)
		{
			Found = false;

			for(std::list<FutureInfo>::reverse_iterator rit = Futures.rbegin(); rit != Futures.rend(); rit++)
			{
				if(GameClockDiff(rit->StartTime) >= rit->Length)
				{
					std::list<FutureInfo>::iterator it = --rit.base();

					rit->Signal(rit->Stream);

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
			Log::Instance.LogErr("Future", "Future Subsystem not started yet!");

			return;
		}

		FutureInfo Future;
		Future.StartTime = GameClockTime();
		Future.Length = 0;
		Future.Stream = Stream;
		Future.Stream.Seek(0);
		Future.Signal.Connect(Ptr);

		Futures.push_back(Future);
	}

	void Future::PostDelayed(FutureFn Ptr, uint32 Length, const MemoryStream &Stream)
	{
		if(!WasStarted)
		{
			Log::Instance.LogErr("Future", "Future Subsystem not started yet!");

			return;
		}

		FutureInfo Future;
		Future.StartTime = GameClockTime();
		Future.Length = Length;
		Future.Stream = Stream;
		Future.Stream.Seek(0);
		Future.Signal.Connect(Ptr);

		Futures.push_back(Future);
	}
}
