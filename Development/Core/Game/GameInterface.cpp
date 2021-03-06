#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	define TAG "Main"

	DisposablePointer<GameInterface> GameInterface::Instance;

	void GameInterface::SetInstance(DisposablePointer<GameInterface> TheInstance)
	{
		Instance = TheInstance;
	}

	bool GameInterface::LoadPackage(const std::string &PackageName)
	{
		DisposablePointer<Stream> TheStream(new FileStream());

		if(!TheStream.AsDerived<FileStream>()->Open(PackageName, StreamFlags::Read))
		{
			Log::Instance.LogErr(TAG, "Unable to open '%s' for reading!", PackageName.c_str());

			return false;
		}

		if(!PackageFileSystemManager::Instance.AddPackage(MakeStringID(PackageName), TheStream))
		{
			Log::Instance.LogErr(TAG, "Unable to load the '%s' Package!", PackageName.c_str());

			return false;
		}

		return true;
	}

	int32 GameInterface::Run(int32 argc, char **argv)
	{
		uint32 OverrideWidth = 0, OverrideHeight = 0;

		for (int32 i = 1; i < argc; i++)
		{
			std::string argument(argv[i]);

			if (argument == "-w" && i + 1 < argc)
			{
				if (1 != sscanf(argv[i + 1], "%u", &OverrideWidth))
				{
					OverrideWidth = 0;
				}
			}
			else if (argument == "-h" && i + 1 < argc)
			{
				if (1 != sscanf(argv[i + 1], "%u", &OverrideHeight))
				{
					OverrideHeight = 0;
				}
			}
			else if (argument == "-mobile")
			{
				PlatformInfo::PlatformType = PlatformType::Mobile;
			}
			else if (argument.find("-sr") == 0)
			{
				char c = argument[strlen("-sr")];

				switch (c)
				{
				case 'n':
					PlatformInfo::ScreenRotation = ScreenRotation::North;

					break;

				case 's':
					PlatformInfo::ScreenRotation = ScreenRotation::South;

					break;

				case 'w':
					PlatformInfo::ScreenRotation = ScreenRotation::West;

					break;

				case 'e':
					PlatformInfo::ScreenRotation = ScreenRotation::East;

					break;

				default:
					Log::Instance.LogWarn(TAG, "Unknown rotation ID '%c'", c);
				}
			}
			else if (argument == "-pc")
			{
				PlatformInfo::PlatformType = PlatformType::PC;
			}
		}

		if (OverrideWidth > 0 && OverrideHeight > 0)
		{
			PlatformInfo::ResolutionOverrideWidth = OverrideWidth;
			PlatformInfo::ResolutionOverrideHeight = OverrideHeight;
		}

		Log::Instance.Register();
		PackageFileSystemManager::Instance.Register();

		InitSubsystems();

		if (!LoadPackage(FileSystemUtils::ResourcesDirectory() + "/Content/Default.package"))
		{
			Log::Instance.LogErr(TAG, "Failed to perform basic initialization, quitting...");

			Instance.Dispose();

			DeInitSubsystems();

			return 1;
		}

		GameClock::Instance.Register();
		FPSCounter::Instance.Register();
		ResourceManager::Instance.Register();
		Console::Instance.Register();
		ObjectModelManager::Instance.Register();
		LuaScriptManager::Instance.Register();
		Future::Instance.Register();
		Profiler::Instance.Register();

#if USE_SOUND
		SoundManager::Instance.Register();
#endif

#if !FLPLATFORM_MOBILE
		FileSystemWatcher::Instance.Register();
#endif

#if USE_GRAPHICS
		RendererManager::Instance.Register();
#endif

		InitSubsystems();

		return 0;
	}

	void GameInterface::OnFixedUpdate()
	{
		if (ObjectModelManager::Instance.Started())
		{
			ObjectModelManager::Instance.EmitMessage(FeatureMessage::FixedUpdate, {});
		}
	}

	void GameInterface::OnFrameUpdate()
	{
		if (ObjectModelManager::Instance.Started())
		{
			ObjectModelManager::Instance.EmitMessage(FeatureMessage::FrameUpdate, {});
		}
	}

#if USE_GRAPHICS
#	if !FLPLATFORM_ANDROID
	void GameInterface::OnGUISandboxTrigger(const std::string &Directory, const std::string &FileName, uint32 Action)
	{
		if(FileName == "GUILayout.resource" || FileName == "DefaultLayout.resource")
			ReloadGUI();
	}
#	endif

	void GameInterface::ReloadGUI()
	{
		RendererManager::Instance.ActiveRenderer()->UIRoot->DeleteAllChildren();

		if (!g_widgets_reader->LoadFile(RendererManager::Instance.ActiveRenderer()->UIRoot, (FileSystemUtils::ResourcesDirectory() + "/GUILayout.resource").c_str()))
		{
			Log::Instance.LogErr(TAG, "Failed to reload the GUI Layout file!");
		}
	}

	Renderer *GameInterface::CreateRenderer(const RenderCreateOptions &Options)
	{
		RendererHandle Handle = 0xFFFFFFFF;

		if(Options.WindowHandle != NULL)
		{
			Log::Instance.LogInfo(TAG, "Creating renderer from window handle '0x%08x'", Options.WindowHandle);

			Handle = RendererManager::Instance.AddRenderer(Options.WindowHandle, Options.Caps);
		}
		else
		{
			std::stringstream str;
			str << "Creating renderer from window data ('" << Options.Title << "' " << Options.Width << "x" << Options.Height << " ";

			if(Options.Style & RendererWindowStyle::Default)
				str << "DEFAULT ";

			if(Options.Style & RendererWindowStyle::FullScreen)
				str << "FULLSCREEN ";

			if(Options.Style & RendererWindowStyle::Popup)
				str << "POPUP ";

			str << ")";

			Log::Instance.LogInfo(TAG, str.str().c_str());

			Handle = RendererManager::Instance.AddRenderer(Options.Title.c_str(), Options.Width, Options.Height, Options.Style, Options.Caps);
		}

		if(Handle == 0xFFFFFFF)
		{
			Log::Instance.LogErr(TAG, "Failed to create a renderer");

			return NULL;
		}

		RendererManager::Instance.SetActiveRenderer(Handle);

		Renderer *TheRenderer = RendererManager::Instance.ActiveRenderer();

		if (BaseResolution != Vector2())
		{
			TheRenderer->BaseResolutionValue = BaseResolution;
		}
		else
		{
			TheRenderer->BaseResolutionValue = TheRenderer->Size();
		}

		TheRenderer->OnFrameStarted.Connect<GameInterface, &GameInterface::OnFrameBegin>(this);
		TheRenderer->OnFrameDraw.Connect<GameInterface, &GameInterface::OnFrameDraw>(this);
		TheRenderer->OnFrameEnded.Connect<GameInterface, &GameInterface::OnFrameEnd>(this);
		TheRenderer->OnResized.Connect<GameInterface, &GameInterface::OnResize>(this);
		TheRenderer->OnResourcesReloaded.Connect<GameInterface, &GameInterface::OnResourcesReloaded>(this);

		if(Options.FrameRate != 0)
		{
			TheRenderer->SetFrameRate(Options.FrameRate);
		}

		Vector2 RendererSize = TheRenderer->Size();

		TheRenderer->SetViewport(0, 0, RendererSize.x, RendererSize.y);

		return TheRenderer;
	}

	void GameInterface::OnFrameDraw(Renderer *TheRenderer, const std::string &ScenePass)
	{
		if (ObjectModelManager::Instance.Started())
		{
			ObjectModelManager::Instance.EmitMessage(FeatureMessage::FrameDraw, { TheRenderer, const_cast<std::string *>(&ScenePass) });
		}
	}

	void GameInterface::OnFrameEnd(Renderer *TheRenderer, const std::string &ScenePass)
	{
		if (ScenePass != SCENEPASS_END)
			return;
	}
#endif

	void GameInterfaceSingleFrame()
	{
		UpdateSubsystems();

		while (GameClockMayPerformFixedTimeStep())
		{
			GameInterface::Instance->OnFixedUpdate();
		}

		GameInterface::Instance->OnFrameUpdate();
	}

	int32 NativeGameInterface::Run(int32 argc, char **argv)
	{
		int32 Result = GameInterface::Run(argc, argv);

		if (Result != 0)
			return Result;

		if(!Initialize(argc, argv))
		{
			Log::Instance.LogErr(TAG, "Failed to perform basic initialization, quitting...");

			Instance.Dispose();

			DeInitSubsystems();

			return 1;
		}

		DisposablePointer<Stream> AutoExecStream(new FileStream());

		if(!AutoExecStream.AsDerived<FileStream>()->Open(FileSystemUtils::PreferredStorageDirectory() + "/autoexec.cfg", StreamFlags::Read | StreamFlags::Text))
		{
			AutoExecStream = PackageFileSystemManager::Instance.GetFile(MakeStringID("/"), MakeStringID("autoexec.cfg"));
		}

		if(AutoExecStream)
		{
			std::vector<std::string> Commands = StringUtils::Split(StringUtils::Strip(AutoExecStream->AsString(), '\r'), '\n');

			for(uint32 i = 0; i < Commands.size(); i++)
			{
				if(Commands[i].length() && Commands[i][0] != '#')
				{
					Console::Instance.RunConsoleCommand(Commands[i]);
				}
			}
		}

		GameClockSetFixedFrameRate(FixedUpdateRate());

#if USE_GRAPHICS
		if (RendererManager::Instance.ActiveRenderer())
		{
			RenderTextUtils::LoadDefaultFont(RendererManager::Instance.ActiveRenderer(), "DefaultFont.ttf");
		}

		if (IsGUISandbox)
		{
#	if !FLPLATFORM_MOBILE
			if (!FileSystemWatcher::Instance.WatchDirectory(FileSystemUtils::ResourcesDirectory()))
			{
				Instance.Dispose();

				DeInitSubsystems();

				return 1;
			}

			FileSystemWatcher::Instance.OnAction.Connect<GameInterface, &GameInterface::OnGUISandboxTrigger>(this);
#	endif

			ReloadGUI();
		}
#endif

#if FLPLATFORM_EMSCRIPTEN
		emscripten_set_main_loop(GameInterfaceSingleFrame, 30, 1);
#else
		for (;;)
		{
			GameInterfaceSingleFrame();

			if(ShouldQuit())
				break;

#if USE_GRAPHICS
			if (PlatformInfo::PlatformType == PlatformType::Mobile && !RendererManager::Instance.Input.HasFocus)
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
#endif
		}
#endif

		if(!DeInitialize())
		{
			Log::Instance.LogErr(TAG, "Failed to properly deinitialize!");

			Instance.Dispose();

			DeInitSubsystems();

			return 1;
		}

		Log::Instance.LogInfo(TAG, "Finished");

		Instance.Dispose();

		DeInitSubsystems();

		return 0;
	}

	ScriptedGameInterface::~ScriptedGameInterface()
	{
		PreInitFunction = luabind::object();
		InitFunction = luabind::object();
		DeInitFunction = luabind::object();
		OnFixedUpdateFunction = luabind::object();
		OnFrameUpdateFunction = luabind::object();
		OnFrameBeginFunction = luabind::object();
		OnFrameDrawFunction = luabind::object();
		OnFrameEndFunction = luabind::object();
		OnResizeFunction = luabind::object();
		OnResourcesReloadedFunction = luabind::object();
		ShouldQuitFunction = luabind::object();
	}

	int32 ScriptedGameInterface::Run(int32 argc, char **argv)
	{
		int32 Result = GameInterface::Run(argc, argv);

		if (Result != 0)
			return Result;

		std::string ConfigurationPackageFileName = FileSystemUtils::ResourcesDirectory() + "/Content/Configuration.package";

		if (!LoadPackage(ConfigurationPackageFileName))
		{
			Instance.Dispose();

			DeInitSubsystems();

			return 1;
		}

		{
			DisposablePointer<Stream> ConfigurationFileStream = PackageFileSystemManager::Instance.GetFile(MakeStringID("/"), MakeStringID("Config.cfg"));
			GenericConfig GameConfig;

			if (!ConfigurationFileStream || !GameConfig.DeSerialize(ConfigurationFileStream))
			{
				Instance.Dispose();

				DeInitSubsystems();

				return 1;
			}

			GenericConfig::Section &PackagesSection = GameConfig.Sections["Packages"];

			for (GenericConfig::Section::ValueMap::iterator it = PackagesSection.Values.begin(); it != PackagesSection.Values.end(); it++)
			{
				if (!LoadPackage(FileSystemUtils::ResourcesDirectory() + "/Content/" + it->second.Content))
				{
					Instance.Dispose();

					DeInitSubsystems();

					return 1;
				}
			}

			GenericConfig::Section &GameSection = GameConfig.Sections["Game"];

			GenericConfig::Section::ValueMap::iterator it = GameSection.Values.find("Title");

			if (it != GameSection.Values.end())
			{
				GameNameValue = it->second.Content;
			}

			it = GameSection.Values.find("UpdateRate");

			if (it != GameSection.Values.end())
			{
				int32 CurrentUpdateRate = UpdateRateValue;

				if (1 != sscanf(it->second.Content.c_str(), "%d", &UpdateRateValue))
				{
					UpdateRateValue = CurrentUpdateRate;
				}
			}

			it = GameSection.Values.find("FrameRate");

			if (it != GameSection.Values.end())
			{
				int32 CurrentFrameRate = FrameRateValue;

				if (1 != sscanf(it->second.Content.c_str(), "%d", &FrameRateValue) || FrameRateValue <= 0)
				{
					FrameRateValue = CurrentFrameRate;
				}
			}

			it = GameSection.Values.find("BaseWidth");

			if (it != GameSection.Values.end())
			{
				int32 Width = 0;

				if (1 == sscanf(it->second.Content.c_str(), "%d", &Width) && Width > 0)
				{
					BaseResolution.x = (f32)Width;
				}
			}

			it = GameSection.Values.find("BaseHeight");

			if (BaseResolution.x > 0 && it != GameSection.Values.end())
			{
				int32 Height = 0;

				if (1 == sscanf(it->second.Content.c_str(), "%d", &Height) && Height > 0)
				{
					BaseResolution.y = (f32)Height;
				}
				else
				{
					BaseResolution = Vector2();
				}
			}
		}

		{
			LuaLib *Libs[] = {
				&FrameworkLib::Instance,
				this
			};

			ScriptInstance = LuaScriptManager::Instance.CreateScript("", Libs, sizeof(Libs) / sizeof(Libs[0]));

			if (!ScriptInstance)
			{
				Instance.Dispose();

				DeInitSubsystems();

				return 1;
			}

			DisposablePointer<Stream> GameStream = PackageFileSystemManager::Instance.GetFile(MakeStringID("/Scripts/Game/"), MakeStringID("Game.lua"));

			if (!GameStream)
			{
				Instance.Dispose();

				DeInitSubsystems();

				return 1;
			}

			ScriptInstance->DoString(GameStream->AsString());

			luabind::object Globals = luabind::globals(ScriptInstance->State);

			InitFunction = Globals["GameInitialize"];
			DeInitFunction = Globals["GameDeInitialize"];
			OnFixedUpdateFunction = Globals["GameFixedUpdate"];
			OnFrameUpdateFunction = Globals["GameFrameUpdate"];
			OnFrameBeginFunction = Globals["GameFrameBegin"];
			OnFrameDrawFunction = Globals["GameFrameDraw"];
			OnFrameEndFunction = Globals["GameFrameEnd"];
			OnResizeFunction = Globals["GameResize"];
			OnResourcesReloadedFunction = Globals["GameResourcesReloaded"];
			ShouldQuitFunction = Globals["GameShouldQuit"];
		}

		bool Success = false;

		if (!InitFunction)
		{
			LuaScriptManager::Instance.LogError("Failed to find script 'GameInitialize' function");

			Instance.Dispose();

			DeInitSubsystems();

			return 1;
		}

		try
		{
			luabind::object Arguments = luabind::newtable(ScriptInstance->State);

			for (int32 i = 0; i < argc; i++)
			{
				Arguments[i + 1] = std::string(argv[i]);
			}

			Success = ProtectedLuaCast<bool>(InitFunction(Arguments));
		}
		catch (std::exception &)
		{
			Instance.Dispose();

			DeInitSubsystems();

			return 1;
		}

		if (!Success)
		{
			LuaScriptManager::Instance.LogError("GameInitialize failed");

			Instance.Dispose();

			DeInitSubsystems();

			return 1;
		}

		DisposablePointer<Stream> AutoExecStream(new FileStream());

		if (!AutoExecStream.AsDerived<FileStream>()->Open(FileSystemUtils::PreferredStorageDirectory() + "/autoexec.cfg", StreamFlags::Read | StreamFlags::Text))
		{
			AutoExecStream = PackageFileSystemManager::Instance.GetFile(MakeStringID("/"), MakeStringID("autoexec.cfg"));
		}

		if (AutoExecStream)
		{
			std::vector<std::string> Commands = StringUtils::Split(StringUtils::Strip(AutoExecStream->AsString(), '\r'), '\n');

			for (uint32 i = 0; i < Commands.size(); i++)
			{
				if (Commands[i].length() && Commands[i][0] != '#')
				{
					Console::Instance.RunConsoleCommand(Commands[i]);
				}
			}
		}

		GameClockSetFixedFrameRate(FixedUpdateRate());

#if USE_GRAPHICS
		if (RendererManager::Instance.ActiveRenderer())
		{
			RenderTextUtils::LoadDefaultFont(RendererManager::Instance.ActiveRenderer(), "DefaultFont.ttf");
		}

		if (IsGUISandbox)
		{
#	if !FLPLATFORM_MOBILE
			if (!FileSystemWatcher::Instance.WatchDirectory(FileSystemUtils::ResourcesDirectory()))
			{
				Instance.Dispose();

				DeInitSubsystems();

				return 1;
			}

			FileSystemWatcher::Instance.OnAction.Connect<GameInterface, &GameInterface::OnGUISandboxTrigger>(this);
#	endif

			ReloadGUI();
		}
#endif

#if FLPLATFORM_EMSCRIPTEN
		emscripten_set_main_loop(GameInterfaceSingleFrame, 30, 1);
#else
		for (;;)
		{
			GameInterfaceSingleFrame();

			if (ShouldQuit())
				break;

#if USE_GRAPHICS
			if (PlatformInfo::PlatformType == PlatformType::Mobile && !RendererManager::Instance.Input.HasFocus)
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
#endif
		}
#endif

		if(DeInitFunction)
		{
			try
			{
				ProtectedLuaCast<bool>(DeInitFunction());
			}
			catch(std::exception &)
			{
				Instance.Dispose();

				DeInitSubsystems();

				return 1;
			}
		}

#if USE_GRAPHICS
		RendererManager::Instance.Input.ClearContexts();
#endif

		Instance.Dispose();

		DeInitSubsystems();

		return 0;
	}

#define STATIC_FUNCTION_CHECK_RETURN_VOID(Function)\
	static bool __STATIC_FN_CHECK__ = !!Function;\
	\
	if(!__STATIC_FN_CHECK__)\
		return;

#define STATIC_FUNCTION_CHECK_RETURN_BOOL(Function, value)\
	static bool __STATIC_FN_CHECK__ = !!Function;\
	\
	if(!__STATIC_FN_CHECK__)\
		return value;

#if USE_GRAPHICS
	void ScriptedGameInterface::OnFrameBegin(Renderer *TheRenderer, const std::string &ScenePass)
	{
		if(ErroredOnFrameBegin)
			return;

		STATIC_FUNCTION_CHECK_RETURN_VOID(OnFrameBeginFunction);

		try
		{
			OnFrameBeginFunction(TheRenderer);
		}
		catch(std::exception &)
		{
			ErroredOnFrameBegin = true;
		}
	}

	void ScriptedGameInterface::OnFrameDraw(Renderer *TheRenderer, const std::string &ScenePass)
	{
		if(ErroredOnFrameDraw)
			return;

		GameInterface::OnFrameDraw(TheRenderer, ScenePass);

		STATIC_FUNCTION_CHECK_RETURN_VOID(OnFrameDrawFunction);

		try
		{
			OnFrameDrawFunction(TheRenderer);
		}
		catch(std::exception &)
		{
			ErroredOnFrameDraw = true;
		}
	}

	void ScriptedGameInterface::OnFrameEnd(Renderer *TheRenderer, const std::string &ScenePass)
	{
		if(ErroredOnFrameEnd)
			return;

		STATIC_FUNCTION_CHECK_RETURN_VOID(OnFrameEndFunction);

		try
		{
			OnFrameEndFunction(TheRenderer);
		}
		catch(std::exception &)
		{
			ErroredOnFrameEnd = true;
		}

		GameInterface::OnFrameEnd(TheRenderer, ScenePass);
	}

	void ScriptedGameInterface::OnResize(Renderer *TheRenderer, uint32 Width, uint32 Height)
	{
		if(ErroredOnResize)
			return;

		STATIC_FUNCTION_CHECK_RETURN_VOID(OnResizeFunction);

		try
		{
			OnResizeFunction(TheRenderer, Width, Height);
		}
		catch(std::exception &)
		{
			ErroredOnResize = true;
		}
	}

	void ScriptedGameInterface::OnResourcesReloaded(Renderer *TheRenderer)
	{
		if(ErroredOnResourcesReloaded)
			return;

		STATIC_FUNCTION_CHECK_RETURN_VOID(OnResourcesReloadedFunction);

		try
		{
			OnResourcesReloadedFunction(TheRenderer);
		}
		catch(std::exception &)
		{
			ErroredOnResourcesReloaded = true;
		}
	}
#endif

	void ScriptedGameInterface::OnFixedUpdate()
	{
		if(ErroredOnFixedUpdate)
			return;

		GameInterface::OnFixedUpdate();

		STATIC_FUNCTION_CHECK_RETURN_VOID(OnFixedUpdateFunction);

		try
		{
			OnFixedUpdateFunction();
		}
		catch(std::exception &)
		{
			ErroredOnFixedUpdate = true;
		}
	}

	void ScriptedGameInterface::OnFrameUpdate()
	{
		if(ErroredOnFrameUpdate)
			return;

		GameInterface::OnFrameUpdate();

		STATIC_FUNCTION_CHECK_RETURN_VOID(OnFrameUpdateFunction);

		try
		{
			OnFrameUpdateFunction();
		}
		catch(std::exception &)
		{
			ErroredOnFrameUpdate = true;
		}
	}

	bool ScriptedGameInterface::ShouldQuit()
	{
		STATIC_FUNCTION_CHECK_RETURN_BOOL(ShouldQuitFunction, true);

		try
		{
			return ProtectedLuaCast<bool>(ShouldQuitFunction()) || QuitFlag;
		}
		catch(std::exception &)
		{
			return true;
		}
	}
}
