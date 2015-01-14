#include "FlamingCore.hpp"
#include "UnitTest.hpp"

using namespace FlamingTorch;

int main(int argc, char **argv)
{
	Log::Instance.Register();
	GameClock::Instance.Register();
	FPSCounter::Instance.Register();
	LuaScriptManager::Instance.Register();
	Console::Instance.Register();
	RendererManager::Instance.Register();
	ResourceManager::Instance.Register();
	ObjectModelManager::Instance.Register();
	PhysicsWorld::Instance.Register();
	SoundManager::Instance.Register();
	FileSystemWatcher::Instance.Register();
	PackageFileSystemManager::Instance.Register();
	Profiler::Instance.Register();
	Future::Instance.Register();

	InitSubsystems();

	int Result = Catch::Session().run(argc, argv);

	DeInitSubsystems();

	return Result;
}
