#pragma once
void InitSubsystems();
void DeInitSubsystems();
void UpdateSubsystems();

#define SUBSYSTEM_PRIORITY_CHECK() \
	if(!WasStarted || Priority != SubsystemPriority)\
		return;

#define SUBSYSTEM_STARTUP_CHECK()  \
	if(WasStarted || Priority != SubsystemPriority)\
		return;

class SubSystem
{
protected:
	bool WasStarted;
	uint32 SubsystemPriority;
private:
	SubSystem(const SubSystem &o);
public:
	//Will be called by priority
	static SimpleDelegate::SimpleDelegate<uint32> OnSubsystemStartUp;
	static SimpleDelegate::SimpleDelegate<uint32> OnSubsystemShutdown;
	static SimpleDelegate::SimpleDelegate<uint32> OnSubsystemUpdate;

	SubSystem(uint32 Priority) : WasStarted(false), SubsystemPriority(Priority) {};

	virtual ~SubSystem() {};

	virtual void StartUp(uint32 Priority)
	{
		if(Priority != SubsystemPriority)
			return;

		WasStarted = true;
	};

	virtual void Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();
		WasStarted = false;
	};

	virtual void Update(uint32 Priority) {};

	//Call this to enable usage of this Subsystem before you call InitSubsystems()
	void Register()
	{
		OnSubsystemStartUp.Connect<SubSystem, &SubSystem::StartUp>(this);
		OnSubsystemShutdown.Connect<SubSystem, &SubSystem::Shutdown>(this);
		OnSubsystemUpdate.Connect<SubSystem, &SubSystem::Update>(this);
	};
};
