#include "FlamingCore.hpp"
#include "DefaultProject.hpp"

using namespace FlamingTorch;

int main(int argc, char **argv)
{
	Log::Instance.Register();
	GameClock::Instance.Register();
	FPSCounter::Instance.Register();
	ResourceManager::Instance.Register();
	Console::Instance.Register();
	ObjectModelManager::Instance.Register();
	LuaScriptManager::Instance.Register();
	PackageFileSystemManager::Instance.Register();

#if !FLPLATFORM_MOBILE
	FileSystemWatcher::Instance.Register();
#endif

#if USE_GRAPHICS
	RendererManager::Instance.Register();
#endif

	DisposablePointer<DefaultProject> InterfaceInstance(new DefaultProject());

	GameInterface::SetInstance(InterfaceInstance);

	return GameInterface::Instance->Run(argc, argv);
};
