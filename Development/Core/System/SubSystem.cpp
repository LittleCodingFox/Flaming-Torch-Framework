#include "FlamingCore.hpp"
namespace FlamingTorch
{
	SimpleDelegate::SimpleDelegate<uint32> SubSystem::OnSubsystemStartUp;
	SimpleDelegate::SimpleDelegate<uint32> SubSystem::OnSubsystemShutdown;
	SimpleDelegate::SimpleDelegate<uint32> SubSystem::OnSubsystemUpdate;

	void InitSubsystems()
	{
		for(uint32 i = 0; i < 64; i++)
		{
			SubSystem::OnSubsystemStartUp(i);
		}
	}

	void DeInitSubsystems()
	{
		for(int32 i = 63; i >= 0; i--)
		{
			SubSystem::OnSubsystemShutdown(i);
		}
	}

	void UpdateSubsystems()
	{
		PROFILE("UpdateSubsystems", StatTypes::Game);

		for(uint32 i = 0; i < 64; i++)
		{
			SubSystem::OnSubsystemUpdate(i);
		}
	}
}
