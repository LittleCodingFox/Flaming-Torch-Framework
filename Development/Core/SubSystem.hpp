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
	static Signal1<uint32> OnSubsystemStartUp;
	static Signal1<uint32> OnSubsystemShutdown;
	static Signal1<uint32> OnSubsystemUpdate;

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
		OnSubsystemStartUp.Connect(this, &SubSystem::StartUp);
		OnSubsystemShutdown.Connect(this, &SubSystem::Shutdown);
		OnSubsystemUpdate.Connect(this, &SubSystem::Update);
	};
};
