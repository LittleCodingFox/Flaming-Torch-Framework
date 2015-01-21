#pragma once

/*!
*	class to run code some time later
*/
class Future : public SubSystem
{
	struct FutureInfo
	{
		uint64 StartTime;
		uint32 Length;
		MemoryStream Stream;

		SimpleDelegate::SimpleDelegate<MemoryStream &> Signal;

		FutureInfo() {}
		FutureInfo(const FutureInfo &o) {}
	};

	std::list<DisposablePointer<FutureInfo> > Futures;

	Future(const Future &);
	Future &operator=(const Future &);
public:
	typedef void (*FutureFn)(MemoryStream &Arguments);

	static Future Instance;

	Future() : SubSystem(FUTURE_PRIORITY) {}

	void StartUp(uint32 Priority);
	void Shutdown(uint32 Priority);
	void Update(uint32 Priority);

	/*!
	*	Registers a function and a memory stream with arguments to be run immediately after the next frame
	*	\param Ptr the function pointer
	*	\param Stream the Memory Stream
	*/
	void Post(FutureFn Ptr, const MemoryStream &Stream = MemoryStream());

	/*!
	*	Registers a function and a memory stream with arguments to be run a while after it is called
	*	\param Ptr the function pointer
	*	\param Delay the delay in miliseconds
	*	\param Stream the Memory Stream
	*/
	void PostDelayed(FutureFn Ptr, uint32 Delay = 0, const MemoryStream &Stream = MemoryStream());
	
	template<class Class> void Post(Class *Instance, void (Class::*Function)(MemoryStream &Arguments), const MemoryStream &Stream = MemoryStream())
	{
		if(!WasStarted)
		{
			Log::Instance.LogErr("Future", "Future Subsystem not started yet!");

			return;
		}

		DisposablePointer<FutureInfo> Future(new FutureInfo());
		Future->StartTime = GameClockTime();
		Future->Length = 0;
		Future->Stream = Stream;
		Future->Stream.Seek(0);
		Future->Signal.Connect(Instance, Function);

		Futures.push_back(Future);
	}

	template<class Class> void PostDelayed(Class *Instance, void (Class::*Function)(MemoryStream &Arguments),
		uint32 Length, const MemoryStream &Stream = MemoryStream())
	{
		if(!WasStarted)
		{
			Log::Instance.LogErr("Future", "Future Subsystem not started yet!");

			return;
		}

		DisposablePointer<FutureInfo> Future(new FutureInfo());
		Future->StartTime = GameClockTime();
		Future->Length = Length;
		Future->Stream = Stream;
		Future->Stream.Seek(0);
		Future->Signal.Connect(Instance, Function);

		Futures.push_back(Future);
	}
};
