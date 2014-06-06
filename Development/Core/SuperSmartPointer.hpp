#pragma once
class SuperSmartPointerCollisionManager
{
public:
	static SuperSmartPointerCollisionManager Instance;

	std::vector<std::pair<void *, void *> > RegisteredPointers;

	void Add(void *Content, void *Pointer)
	{
#if LOG_SSP_BEHAVIOUR
		for(int32 i = 0; i < RegisteredPointers.size(); i++)
		{
			if(RegisteredPointers[i].second == Pointer)
			{
				Log::Instance.LogErr("Core", "Detected double add of an object 0x%08x "
					"with content 0x%08x (secondary add was found on 0x%08x)",
					Pointer, RegisteredPointers[i].first, Content);
				DEBUG_BREAK;
			};
		};

		RegisteredPointers.push_back(std::pair<void *, void *>(Content, Pointer));
#endif
	};

	void Remove(void *Content)
	{
#if LOG_SSP_BEHAVIOUR
		for(int32 i = 0; i < RegisteredPointers.size(); i++)
		{
			if(RegisteredPointers[i].first == Content)
			{
				RegisteredPointers.erase(RegisteredPointers.begin() + i);

				return;
			};
		};
#endif
	};
};

class SuperSmartPointerOperationCounter : public SubSystem
{
public:
	int32 Counter, AllocCounter;
	uint64 LastUpdateTime;

	static SuperSmartPointerOperationCounter Instance;

	SuperSmartPointerOperationCounter() : SubSystem(SSP_OPERATIONCOUNTER_PRIORITY), Counter(0), LastUpdateTime(0),
		AllocCounter(0) {};

	void Report(bool Alloc)
	{
		(Alloc ? AllocCounter : Counter)++;
	};

	void StartUp(uint32 Priority)
	{
		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo("SSPPointerOperationCounter", "Super Smart Pointer Operation Counter Subsystem starting..");
	};

	void Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo("SSPPointerOperationCounter", "Super Smart Pointer Operation Counter Subsystem terminating..");

		SubSystem::Shutdown(Priority);
	};

	void Update(uint32 Priority)
	{
		SubSystem::Update(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		if(GameClockDiffNoPause(LastUpdateTime) >= 1000)
		{
			LastUpdateTime += 1000;

			Log::Instance.LogDebug("SSPPointerOperationCounter", "%d (%.2f avg) SSP operations (%d (%.2f avg) allocs)", Counter, Counter / 1000.f, AllocCounter,
				AllocCounter / 1000.f);

			Counter = AllocCounter = 0;
		};
	};
};

/*!
	Smart Pointer implementation using Observer Pattern and implicit Destruction
*/
template<typename type>
class SuperSmartPointer
{
private:
	template<typename OutType>
	friend class SuperSmartPointer;

	class SuperSmartPointerContainer
	{
	public:
		friend class SuperSmartPointer<type>;
		typedef std::vector<SuperSmartPointer *> ObserverList;
		ObserverList Observers;
		type *Object;

		SuperSmartPointerContainer() : Object(NULL)
		{
		};

		void Add(SuperSmartPointer *Observer)
		{
			SuperSmartPointerOperationCounter::Instance.Report(false);

#if LOG_SSP_BEHAVIOUR || LOG_SSP_ALLOCS
			Log::Instance.LogDebug("Core", "SSP Content 0x%08x with Object 0x%08x Adding Observer 0x%08x", this, Object, Observer);
#endif

			Observers.push_back(Observer);
		};

		void Remove(SuperSmartPointer *Observer)
		{
			SuperSmartPointerOperationCounter::Instance.Report(false);

#if LOG_SSP_BEHAVIOUR || LOG_SSP_ALLOCS
			Log::Instance.LogDebug("Core", "SSP Content 0x%08x Removing Observer 0x%08x", this, Observer);
#endif

			if(Observers.size())
			{
				for(typename ObserverList::iterator it = Observers.begin(); it != Observers.end(); it++)
				{
					if(*it == Observer)
					{
						Observer->ContainerDisposed();

						it = Observers.erase(it);

						if(Observers.size() == 0)
						{
#if LOG_SSP_BEHAVIOUR
							Log::Instance.LogDebug("Core", " -> Observer Size is 0, Disposing Object 0x%08x...", Object);
#endif

							SuperSmartPointerCollisionManager::Instance.Remove(this);
							SuperSmartPointerOperationCounter::Instance.Report(true);

							if(Object)
							{
								delete Object;
								Object = NULL;
							};

							delete this;
						};

						break;
					};
				};
			};
		};

		void Dispose()
		{
			SuperSmartPointerOperationCounter::Instance.Report(true);

#if LOG_SSP_BEHAVIOUR || LOG_SSP_ALLOCS
			Log::Instance.LogDebug("Core", "SSP Content Disposing object: 0x%08x; Object: 0x%08x", this, Object);
#endif
			int32 Count = 0;

			while(Observers.size())
			{
				Count++;

#if LOG_SSP_BEHAVIOUR
				Log::Instance.LogDebug("Core", " -> Marking Content as NULL on Observer #%d (0x%08x)", Count, (*Observers.begin()));
#endif

				(*Observers.begin())->ContainerDisposed();
				Observers.erase(Observers.begin());
			};

			if(Object)
			{
#if LOG_SSP_BEHAVIOUR
				Log::Instance.LogDebug("Core", " -> Content Object is non-NULL 0x%08x, deleting...", Object);
#endif

				SuperSmartPointerCollisionManager::Instance.Remove(this);

				delete Object;
				Object = NULL;
			};

			delete this;
		};

		void Replace(type *NewObject, bool Destroy)
		{
#if LOG_SSP_BEHAVIOUR
			Log::Instance.LogDebug("Core", "SSP Content Resetting object: 0x%08x; Object: 0x%08x; Destroy: %s", this, Object,
				Destroy ? "TRUE" : "FALSE");
#endif

			if(Object && Destroy)
			{
				SuperSmartPointerCollisionManager::Instance.Remove(this);
				SuperSmartPointerOperationCounter::Instance.Report(true);

				delete Object;
			};

			Object = NewObject;
		};
	};

	SuperSmartPointerContainer *Content;

	void ContainerDisposed()
	{
#if LOG_SSP_BEHAVIOUR
		Log::Instance.LogDebug("Core", "Marking Container Disposed for object 0x%08x from instance 0x%08x", Content, this);
#endif

		Content = NULL;
	};
public:
	SuperSmartPointer() : Content(NULL) {};
	explicit SuperSmartPointer(type *Object) : Content(NULL)
	{
#if LOG_SSP_BEHAVIOUR
		Log::Instance.LogDebug("Core", "SSP being created 0x%08x with object 0x%08x", this, Object);
#endif

		if(Object)
		{
			Content = new SuperSmartPointerContainer();
			Content->Object = Object;
			Content->Add(this);

			SuperSmartPointerOperationCounter::Instance.Report(true);

#if LOG_SSP_BEHAVIOUR
			SuperSmartPointerCollisionManager::Instance.Add(Content, Object);
#endif
		};
	};

	SuperSmartPointer<type>(const SuperSmartPointer<type> &o) : Content(NULL)
	{
#if LOG_SSP_BEHAVIOUR
		Log::Instance.LogDebug("Core", "SSP being copied 0x%08x with target 0x%08x", this, &o);
#endif

		*this = o;
	};

	~SuperSmartPointer()
	{
#if LOG_SSP_BEHAVIOUR
		Log::Instance.LogDebug("Core", "SSP being destroyed: 0x%08x; Content: %08x", this, Content);
#endif

		if(Content)
			Content->Remove(this);

		Content = NULL;
	};

	int32 ObserverCount()
	{
		return Content == NULL ? 0 : Content->Observers.size();
	};

	SuperSmartPointer<type> &operator=(const SuperSmartPointer<type> &o)
	{
#if LOG_SSP_BEHAVIOUR
		Log::Instance.LogDebug("Core", "SSP being assigned: 0x%08x; o: %08x; ContentMe: 0x%08x; ContentThem: 0x%08x",
			this, &o, Content, o.Content);
#endif

		if(Content == o.Content || this == &o)
			return *this;

		if(Content) {
			Content->Remove(this);
			Content = NULL;
		};

		if(!o.Content)
			return *this;

		Content = o.Content;
		Content->Add(this);

		return *this;
	};

	template<typename OutType>
	operator SuperSmartPointer<OutType>()
	{
		if(!Content)
			return SuperSmartPointer<OutType>();

#if CHECK_SSP_CONVERSIONS
		{
			OutType *t = dynamic_cast<OutType *>(Content->Object);

			if(!t)
				return SuperSmartPointer<OutType>();
		}
#endif

		SuperSmartPointer<OutType> Out;
		Out.Content = reinterpret_cast<typename SuperSmartPointer<OutType>::SuperSmartPointerContainer *>(Content);
		Out.Content->Add(&Out);

		SuperSmartPointerOperationCounter::Instance.Report(true);

		return Out;
	}

	/*!
		returns a Derived object from the base object contained within
	*/
	template<typename OutType> OutType *AsDerived()
	{
		type *t = Get();

#if CHECK_SSP_CONVERSIONS
		return dynamic_cast<OutType *>(t);
#endif

		return static_cast<OutType *>(t);
	};

	inline const type& operator*() const
	{
		return *Get();
	};

	inline type& operator*()
	{
		return *Get();
	};

	inline operator bool() const
	{
		return Get() != 0;
	};

	inline const type *operator->() const
	{
		return Get();
	};

	inline type *operator->()
	{
		return Get();
	};

	inline const type *Get() const
	{
		return Content ? Content->Object : NULL;
	};

	inline type *Get()
	{
		return Content ? Content->Object : NULL;
	};

	bool operator==(const SuperSmartPointer<type> &o)
	{
		return &o == this || o.Get() == Get();
	};

	bool operator!=(const SuperSmartPointer<type> &o)
	{
		return !(&o == this || o.Get() == Get());
	};

	operator bool()
	{
		return !!Get();
	};

	operator type*()
	{
		return Get();
	};

	void Reset(type *Object)
	{
#if LOG_SSP_BEHAVIOUR
		Log::Instance.LogDebug("Core", "SSP Resetting object: 0x%08x; Content: 0x%08x; Object: 0x%08x", this, Content, Object);
#endif

		if(Content)
		{
			Content->Remove(this);
			Content = NULL;
		};

		if(Object)
		{
			Content = new SuperSmartPointerContainer();
			Content->Object = Object;
			Content->Add(this);
			SuperSmartPointerOperationCounter::Instance.Report(true);

#if LOG_SSP_BEHAVIOUR
			SuperSmartPointerCollisionManager::Instance.Add(Content, Object);
#endif
		};
	};

	void Replace(type *Object, bool Destroy = true)
	{
#if LOG_SSP_BEHAVIOUR
		Log::Instance.LogDebug("Core", "SSP Replacing object: 0x%08x; Content: 0x%08x; Object: 0x%08x; Destroy: %s", this, Content, Object,
			Destroy ? "TRUE" : "FALSE");
#endif

		if(Content)
		{
			Content->Replace(Object, Destroy);
		}
		else
		{
			SuperSmartPointerOperationCounter::Instance.Report(true);

			Content = new SuperSmartPointerContainer();
			Content->Object = Object;
			Content->Add(this);

#if LOG_SSP_BEHAVIOUR
			SuperSmartPointerCollisionManager::Instance.Add(Content, Object);
#endif
		};
	};

	/*!
		Destroys the contained object
	*/
	void Dispose()
	{
#if LOG_SSP_BEHAVIOUR
		Log::Instance.LogDebug("Core", "SSP Disposing object: 0x%08x; Content: 0x%08x", this, Content);
#endif

		if(Content)
		{
			SuperSmartPointerOperationCounter::Instance.Report(true);

			Content->Dispose();
		};
	};
};
