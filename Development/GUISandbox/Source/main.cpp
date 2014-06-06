#include "FlamingCore.hpp"
#include "Sandbox.hpp"
#include <FileWatcher/FileWatcher.h>

using namespace FlamingTorch;

#define TAG "GUISandbox"

// create the file watcher object
FW::FileWatcher fileWatcher;

class SandboxInterface : public NativeGameInterface
{
public:
	const std::string &GameName()
	{
		static std::string Out = "GUISandbox";
		return Out;
	};

	int32 FixedUpdateRate()
	{
		return 60;
	};

	static void ReloadGUI()
	{
		RendererManager::Instance.ActiveRenderer()->UI->ClearLayouts();

		SuperSmartPointer<FileStream> InputStream(new FileStream());

		if(!InputStream->Open(DirectoryInfo::ResourcesDirectory() + "GUILayout.resource", StreamFlags::Read | StreamFlags::Text))
			return;

		if(!RendererManager::Instance.ActiveRenderer()->UI->LoadLayouts(InputStream))
		{
			Log::Instance.LogErr("Main", "Failed to load our layout!");
		};
	};

	bool Initialize()
	{
		DevelopmentBuild = true;

		if(!LoadPackage(DirectoryInfo::ActiveDirectory() + "/Default.package"))
			return false;

		RenderCreateOptions RenderOptions;
		RenderOptions.Title = "GUISandbox";
		RenderOptions.Width = GAME_WINDOW_WIDTH;
		RenderOptions.Height = GAME_WINDOW_HEIGHT;
		RenderOptions.Style = sf::Style::Default;

		GenericConfig Config;

		SuperSmartPointer<FileStream> ConfigStream(new FileStream());

		if(ConfigStream->Open(DirectoryInfo::PreferredStorageDirectory() + "/Config.cfg", StreamFlags::Read | StreamFlags::Text) &&
			Config.DeSerialize(ConfigStream))
		{
			RenderOptions.Width = Config.GetInt("Graphics", "Width", GAME_WINDOW_WIDTH);
			RenderOptions.Height = Config.GetInt("Graphics", "Height", GAME_WINDOW_HEIGHT);
		};

		Config.SetValue("Graphics", "Width", GenericConfig::IntValue(RenderOptions.Width).Content.c_str());
		Config.SetValue("Graphics", "Height", GenericConfig::IntValue(RenderOptions.Height).Content.c_str());

		if(ConfigStream->Open(DirectoryInfo::PreferredStorageDirectory() + "/Config.cfg", StreamFlags::Write | StreamFlags::Text))
		{
			Config.Serialize(ConfigStream);
		};

		RendererManager::Renderer *Renderer = CreateRenderer(RenderOptions);

		if(NULL == Renderer)
			return false;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, Renderer->Size().x, Renderer->Size().y, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);

		Renderer->EnableState(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		RenderTextUtils::LoadDefaultFont("sans.ttf");

		{
			SuperSmartPointer<Stream> TheStream = PackageFileSystemManager::Instance.GetFile(MakeStringID("/UIThemes/PolyCode/"), MakeStringID("skin.cfg"));
			SuperSmartPointer<GenericConfig> Skin(new GenericConfig);

			if(!TheStream || !Skin->DeSerialize(TheStream))
			{
				Log::Instance.LogErr("Main", "Unable to load the PolyCode GUI Skin!");

				DeInitSubsystems();

				return false;
			};

			Renderer->UI->SetSkin(Skin);
		};

		ReloadGUI();

		return true;
	};
	
	bool DeInitialize()
	{
		return true;
	};
	
	void OnFixedUpdate()
	{
	};

	void OnFrameUpdate()
	{
		if((RendererManager::Instance.Input.Keys[sf::Keyboard::LAlt].Pressed || RendererManager::Instance.Input.Keys[sf::Keyboard::RAlt].Pressed) &&
			RendererManager::Instance.Input.Keys[sf::Keyboard::Return].JustPressed)
		{
			RendererManager::Instance.ActiveRenderer()->SetFullScreen(RendererManager::Instance.ActiveRenderer()->Size(),
				!RendererManager::Instance.ActiveRenderer()->GetFullScreen());
		};

		fileWatcher.update();
	};
	
	bool ShouldQuit()
	{
		return !RendererManager::Instance.RequestFrame();
	};

	void OnFrameBegin(RendererManager::Renderer *TheRenderer)
	{
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);
	};

	void OnFrameDraw(RendererManager::Renderer *TheRenderer)
	{
	};

	void OnFrameEnd(RendererManager::Renderer *TheRenderer)
	{
		GameInterface::OnFrameEnd(TheRenderer);
	};

	void OnResize(RendererManager::Renderer *TheRenderer, uint32 Width, uint32 Height)
	{
		glViewport(0, 0, Width, Height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, (f32)Width, (f32)Height, 0, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		ReloadGUI();
	};

	void OnResourcesReloaded(RendererManager::Renderer *TheRenderer)
	{
		OnResize(TheRenderer, (uint32)TheRenderer->Size().x, (uint32)TheRenderer->Size().y);
	};
};

class UpdateListener : public FW::FileWatchListener
{
public:
	void handleFileAction(FW::WatchID watchid, const FW::String& dir, const FW::String& filename,
		FW::Action action)
	{
		if(filename == "GUILayout.resource")
			SandboxInterface::ReloadGUI();
	}
};

int main(int argc, char **argv)
{
	//Register Subsystems
	Log::Instance.Register();
	GameClock::Instance.Register();
	RendererManager::Instance.Register();
	FPSCounter::Instance.Register();
	PackageFileSystemManager::Instance.Register();
	Future::Instance.Register();
	ResourceManager::Instance.Register();
	SuperSmartPointerOperationCounter::Instance.Register();
	LuaScriptManager::Instance.Register();
	Console::Instance.Register();

	FW::WatchID watchID = fileWatcher.addWatch(DirectoryInfo::ResourcesDirectory(), new UpdateListener());

	GameInterface::SetInstance(SuperSmartPointer<GameInterface>(new SandboxInterface()));

	return GameInterface::Instance->Run(argc, argv);
};
