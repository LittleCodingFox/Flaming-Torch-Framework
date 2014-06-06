#include "FlamingCore.hpp"
namespace FlamingTorch
{
	//TODO: Maybe move this somewhere else?
	SuperSmartPointerOperationCounter SuperSmartPointerOperationCounter::Instance;

	Signal1<uint32> SubSystem::OnSubsystemStartUp;
	Signal1<uint32> SubSystem::OnSubsystemShutdown;
	Signal1<uint32> SubSystem::OnSubsystemUpdate;

	void FLAMING_API InitSubsystems()
	{
		for(uint32 i = 0; i < 64; i++)
		{
			SubSystem::OnSubsystemStartUp(i);
		};
	};

	void FLAMING_API DeInitSubsystems()
	{
		for(int32 i = 63; i >= 0; i--)
		{
			SubSystem::OnSubsystemShutdown(i);
		};
	};

	void FLAMING_API UpdateSubsystems()
	{
		PROFILE("UpdateSubsystems", StatTypes::Game);

		for(uint32 i = 0; i < 64; i++)
		{
			SubSystem::OnSubsystemUpdate(i);
		};
	};
};
