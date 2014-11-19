#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	define TAG "Main"

	SuperSmartPointer<GameInterface> GameInterface::Instance;

	void GameInterface::SetInstance(SuperSmartPointer<GameInterface> TheInstance)
	{
		Instance = TheInstance;
	};

	bool GameInterface::LoadPackage(const std::string &PackageName)
	{
		SuperSmartPointer<Stream> TheStream(new FileStream());

		if(!TheStream.AsDerived<FileStream>()->Open(PackageName, StreamFlags::Read))
		{
			Log::Instance.LogErr(TAG, "Unable to open '%s' for reading!", PackageName.c_str());

			return false;
		};

		if(!PackageFileSystemManager::Instance.AddPackage(MakeStringID(PackageName), TheStream))
		{
			Log::Instance.LogErr(TAG, "Unable to load the '%s' Package!", PackageName.c_str());

			return false;
		};

		return true;
	};

#if USE_GRAPHICS
#	if !FLPLATFORM_ANDROID
	void GameInterface::OnGUISandboxTrigger(const std::string &Directory, const std::string &FileName, uint32 Action)
	{
		if(FileName == "GUILayout.resource" || FileName == "DefaultLayout.resource")
			ReloadGUI();
	};
#	endif

	void GameInterface::ReloadGUI()
	{
		RendererManager::Instance.ActiveRenderer()->UI->ClearLayouts();

		SuperSmartPointer<FileStream> InputStream(new FileStream());

		if(!InputStream->Open(FileSystemUtils::ResourcesDirectory() + "DefaultLayout.resource", StreamFlags::Read | StreamFlags::Text) ||
			!RendererManager::Instance.ActiveRenderer()->UI->LoadLayouts(InputStream, SuperSmartPointer<UIPanel>(), true))
		{
			Log::Instance.LogErr(TAG, "Failed to reload our Default GUI Layouts!");

			return;
		};

		if(!InputStream->Open(FileSystemUtils::ResourcesDirectory() + "GUILayout.resource", StreamFlags::Read | StreamFlags::Text) ||
			!RendererManager::Instance.ActiveRenderer()->UI->LoadLayouts(InputStream))
		{
			Log::Instance.LogErr(TAG, "Failed to reload our GUI Layouts!");

			return;
		};
	};

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
		};

		if(Handle == 0xFFFFFFF)
		{
			Log::Instance.LogErr(TAG, "Failed to create a renderer");

			return NULL;
		};

		RendererManager::Instance.SetActiveRenderer(Handle);

		Renderer *TheRenderer = RendererManager::Instance.ActiveRenderer();

		if(BaseResolution != Vector2())
			TheRenderer->BaseResolutionValue = BaseResolution;

		TheRenderer->OnFrameStarted.Connect<GameInterface, &GameInterface::OnFrameBegin>(this);
		TheRenderer->OnFrameDraw.Connect<GameInterface, &GameInterface::OnFrameDraw>(this);
		TheRenderer->OnFrameEnded.Connect<GameInterface, &GameInterface::OnFrameEnd>(this);
		TheRenderer->OnResized.Connect<GameInterface, &GameInterface::OnResize>(this);
		TheRenderer->OnResourcesReloaded.Connect<GameInterface, &GameInterface::OnResourcesReloaded>(this);

		if(Options.FrameRate != 0)
		{
			TheRenderer->SetFrameRate(Options.FrameRate);
		};

		Vector2 RendererSize = TheRenderer->Size();

		TheRenderer->SetViewport(0, 0, RendererSize.x, RendererSize.y);

		return TheRenderer;
	};

	void GameInterface::OnFrameEnd(Renderer *TheRenderer)
	{
		bool PushOrtho = TheRenderer->MatrixStackSize() == 0;

		if(PushOrtho)
		{
			TheRenderer->PushMatrices();
			TheRenderer->SetProjectionMatrix(Matrix4x4::OrthoMatrixRH(0, TheRenderer->Size().x, TheRenderer->Size().y, 0, -1, 1));
		};

		{
			PROFILE("Update UI", StatTypes::Rendering);
			TheRenderer->UI->Update();
		};

		{
			PROFILE("Render UI", StatTypes::Rendering);
			TheRenderer->UI->Draw(TheRenderer);
		};

		static std::stringstream str;

		if(!!Console::Instance.GetVariable("r_drawrenderstats") && Console::Instance.GetVariable("r_drawrenderstats")->UintValue != 0)
		{
			str.str("");

			const RendererFrameStats &Stats = TheRenderer->FrameStats();

			str << "Renderer: " << Stats.RendererName << " version " << Stats.RendererVersion << " on " << CoreUtils::PlatformString() << "\n" << Stats.RendererCustomMessage << (Stats.RendererCustomMessage.length() ? "\n\n" : "\n");
			str << "Frame Stats:\nDraw calls: " << Stats.DrawCalls << "/" << Stats.DrawCalls + Stats.SkippedDrawCalls << "\nVertex Count: " << Stats.VertexCount << "\nTexture Changes: " << Stats.TextureChanges << "\nMatrix Changes: " << Stats.MatrixChanges << 
				"\nClipping Changes: " << Stats.ClippingChanges << "\nState Changes: " << Stats.StateChanges << "\nActive Resources: " << Stats.TotalResources << " (" << Stats.TotalResourceUsage << " MB)\n";

			RenderTextUtils::RenderText(TheRenderer, str.str(), TextParams().Font(TheRenderer->UI->GetDefaultFont()).FontSize(TheRenderer->UI->GetDefaultFontSize())
				.Color(Vector4(1, 1, 1, 1)).BorderColor(Vector4(0, 0, 0, 1)).BorderSize(1).Position(Vector2(0, 0)));
		};

		if(DevelopmentBuild)
		{
#if _DEBUG
#	define GAMEINTERFACE_BUILD_TYPE "DEBUG"
#elif FLGAME_RELEASE
#	define GAMEINTERFACE_BUILD_TYPE "GAME_RELEASE"
#else
#	define GAMEINTERFACE_BUILD_TYPE "RELEASE"
#endif
			str.str("");

			str << GameName() << " (" << GAMEINTERFACE_BUILD_TYPE << ") - " << FPSCounter::Instance.FPS() << " FPS (" << 1000.f / FPSCounter::Instance.FPS() << " ms)";

			RenderTextUtils::RenderText(TheRenderer, str.str(), TextParams().Font(TheRenderer->UI->GetDefaultFont()).FontSize(TheRenderer->UI->GetDefaultFontSize())
				.Color(Vector4(1, 1, 1, 1)).BorderColor(Vector4(0, 0, 0, 1)).BorderSize(1).Position(Vector2(0, TheRenderer->Size().y - 20.0f)));

			static SuperSmartPointer<Texture> Logo;
			static bool TriedLoadLogo = false;

			if(Logo.Get() == NULL && !TriedLoadLogo)
			{
				TriedLoadLogo = true;

				Logo = ResourceManager::Instance.GetTextureFromPackage("/", "torch_small.png");
			};

			if(Logo.Get())
			{
				Sprite TheSprite;
				TheSprite.SpriteTexture = Logo;
				TheSprite.Options.Position(TheRenderer->Size() - Logo->Size());

				TheSprite.Draw(TheRenderer);
			};
		};

		if(PushOrtho)
		{
			TheRenderer->PopMatrices();
		};
	};
#endif

	int32 NativeGameInterface::Run(int32 argc, char **argv)
	{
		FirstFrame = true;
		InitSubsystems();

		if(!Initialize(argc, argv))
		{
			Log::Instance.LogErr(TAG, "Failed to perform basic initialization, quitting...");

			Instance.Dispose();

			DeInitSubsystems();

			return 1;
		};

		SuperSmartPointer<Stream> AutoExecStream(new FileStream());

		if(!AutoExecStream.AsDerived<FileStream>()->Open(FileSystemUtils::PreferredStorageDirectory() + "/autoexec.cfg", StreamFlags::Read | StreamFlags::Text))
		{
			AutoExecStream = PackageFileSystemManager::Instance.GetFile(MakeStringID("/"), MakeStringID("autoexec.cfg"));
		};

		if(AutoExecStream)
		{
			std::vector<std::string> Commands = StringUtils::Split(StringUtils::Strip(AutoExecStream->AsString(), '\r'), '\n');

			for(uint32 i = 0; i < Commands.size(); i++)
			{
				if(Commands[i].length() && Commands[i][0] != '#')
				{
					Console::Instance.RunConsoleCommand(Commands[i]);
				};
			};
		};

		GameClockSetFixedFrameRate(FixedUpdateRate());

		for(;;)
		{
			UpdateSubsystems();

			while(GameClockMayPerformFixedTimeStep())
			{
				OnFixedUpdate();
			};

			OnFrameUpdate();

			if(ShouldQuit())
				break;

			if(FirstFrame)
			{
				FirstFrame = false;

#if USE_GRAPHICS
				if(IsGUISandbox)
				{
#	if !FLPLATFORM_ANDROID
					if(!FileSystemWatcher::Instance.WatchDirectory(FileSystemUtils::ResourcesDirectory()))
					{
						Instance.Dispose();

						DeInitSubsystems();

						return 1;
					};

					FileSystemWatcher::Instance.OnAction.Connect<GameInterface, &GameInterface::OnGUISandboxTrigger>(this);
#	endif

					ReloadGUI();
				};
#endif
			};

#if USE_GRAPHICS
			if(IsMobile && !RendererManager::Instance.Input.HasFocus)
				sf::sleep(sf::milliseconds(1000));
#endif
		};

		if(!DeInitialize())
		{
			Log::Instance.LogErr(TAG, "Failed to properly deinitialize!");

			Instance.Dispose();

			DeInitSubsystems();

			return 1;
		};

		Log::Instance.LogInfo(TAG, "Finished");

		Instance.Dispose();

		DeInitSubsystems();

		return 0;
	};

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
	};

	int32 ScriptedGameInterface::Run(int32 argc, char **argv)
	{
		FirstFrame = true;
		PackageFileSystemManager::Instance.Register();

		InitSubsystems();
        
        std::string DefaultPackageFileName = FileSystemUtils::ResourcesDirectory() + "/Content/Default.package";
        
        std::string ConfigurationPackageFileName = FileSystemUtils::ResourcesDirectory() + "/Content/Configuration.package";

		if(!LoadPackage(DefaultPackageFileName))
        {
			Instance.Dispose();
            
			DeInitSubsystems();
            
			return 1;
        }
        else if(!LoadPackage(ConfigurationPackageFileName))
        {
			Instance.Dispose();
            
			DeInitSubsystems();
            
			return 1;
        };

		{
			SuperSmartPointer<Stream> ConfigurationFileStream = PackageFileSystemManager::Instance.GetFile(MakeStringID("/"), MakeStringID("Config.cfg"));
			GenericConfig GameConfig;

			if(!ConfigurationFileStream || !GameConfig.DeSerialize(ConfigurationFileStream))
			{
				Instance.Dispose();

				DeInitSubsystems();

				return 1;
			};

			GenericConfig::Section &PackagesSection = GameConfig.Sections["Packages"];

			for(GenericConfig::Section::ValueMap::iterator it = PackagesSection.Values.begin(); it != PackagesSection.Values.end(); it++)
			{
				if(!LoadPackage(FileSystemUtils::ResourcesDirectory() + "/Content/" + it->second.Content))
				{
					Instance.Dispose();

					DeInitSubsystems();

					return 1;
				};
			};

			GenericConfig::Section &GameSection = GameConfig.Sections["Game"];

			GenericConfig::Section::ValueMap::iterator it = GameSection.Values.find("Title");

			if(it != GameSection.Values.end())
			{
				GameNameValue = it->second.Content;
			};

			it = GameSection.Values.find("UpdateRate");

			if(it != GameSection.Values.end())
			{
				int32 CurrentUpdateRate = UpdateRateValue;

				if(1 != sscanf(it->second.Content.c_str(), "%d", &UpdateRateValue))
				{
					UpdateRateValue = CurrentUpdateRate;
				};
			};

			it = GameSection.Values.find("FrameRate");

			if(it != GameSection.Values.end())
			{
				int32 CurrentFrameRate = FrameRateValue;

				if(1 != sscanf(it->second.Content.c_str(), "%d", &FrameRateValue) || FrameRateValue <= 0)
				{
					FrameRateValue = CurrentFrameRate;
				};
			};

			it = GameSection.Values.find("BaseWidth");

			if(it != GameSection.Values.end())
			{
				int32 Width = 0;

				if(1 == sscanf(it->second.Content.c_str(), "%d", &Width) && Width > 0)
				{
					BaseResolution.x = (f32)Width;
				};
			};

			it = GameSection.Values.find("BaseHeight");

			if(BaseResolution.x > 0 && it != GameSection.Values.end())
			{
				int32 Height = 0;

				if(1 == sscanf(it->second.Content.c_str(), "%d", &Height) && Height > 0)
				{
					BaseResolution.y = (f32)Height;
				}
				else
				{
					BaseResolution = Vector2();
				};
			};
		};

		LuaScriptManager::Instance.Register();

		InitSubsystems();

		{
			LuaLib *Libs[] = {
				&FrameworkLib::Instance,
				this
			};

			ScriptInstance = LuaScriptManager::Instance.CreateScript("", Libs, sizeof(Libs) / sizeof(Libs[0]));

			if(!ScriptInstance)
			{
				Instance.Dispose();

				DeInitSubsystems();

				return 1;
			};

			SuperSmartPointer<Stream> GameStream = PackageFileSystemManager::Instance.GetFile(MakeStringID("/Scripts/Game/"), MakeStringID("Game.lua"));

			if(!GameStream)
			{
				Instance.Dispose();

				DeInitSubsystems();

				return 1;
			};

			ScriptInstance->DoString(GameStream->AsString());

			luabind::object Globals = luabind::globals(ScriptInstance->State);

			PreInitFunction = Globals["GamePreInitialize"];
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
		};

		bool Success = false;

		if(!PreInitFunction)
		{
			LuaScriptManager::Instance.LogError("Failed to find script 'GamePreInitialize' function");

			Instance.Dispose();

			DeInitSubsystems();

			return 1;
		};

		try
		{
			Success = ProtectedLuaCast<bool>(PreInitFunction());
		}
		catch(std::exception &e)
		{
			Instance.Dispose();

			DeInitSubsystems();

			return 1;
		};

		if(!Success)
		{
			LuaScriptManager::Instance.LogError("GamePreInitialize failed");

			Instance.Dispose();

			DeInitSubsystems();

			return 1;
		};

		InitSubsystems();

		Success = false;

		if(!InitFunction)
		{
			LuaScriptManager::Instance.LogError("Failed to find script 'GameInitialize' function");

			Instance.Dispose();

			DeInitSubsystems();

			return 1;
		};

		try
		{
			luabind::object Arguments = luabind::newtable(ScriptInstance->State);

			for(int32 i = 0; i < argc; i++)
			{
				Arguments[i + 1] = std::string(argv[i]);
			};

			Success = ProtectedLuaCast<bool>(InitFunction(Arguments));
		}
		catch(std::exception &e)
		{
			Instance.Dispose();

			DeInitSubsystems();

			return 1;
		};

		if(!Success)
		{
			LuaScriptManager::Instance.LogError("GameInitialize failed");

			Instance.Dispose();

			DeInitSubsystems();

			return 1;
		};

		SuperSmartPointer<Stream> AutoExecStream(new FileStream());

		if(!AutoExecStream.AsDerived<FileStream>()->Open(FileSystemUtils::PreferredStorageDirectory() + "/autoexec.cfg", StreamFlags::Read | StreamFlags::Text))
		{
			AutoExecStream = PackageFileSystemManager::Instance.GetFile(MakeStringID("/"), MakeStringID("autoexec.cfg"));
		};

		if(AutoExecStream)
		{
			std::vector<std::string> Commands = StringUtils::Split(StringUtils::Strip(AutoExecStream->AsString(), '\r'), '\n');

			for(uint32 i = 0; i < Commands.size(); i++)
			{
				if(Commands[i].length() && Commands[i][0] != '#')
				{
					Console::Instance.RunConsoleCommand(Commands[i]);
				};
			};
		};

		GameClockSetFixedFrameRate(FixedUpdateRate());

#if USE_GRAPHICS
		if(RendererManager::Instance.ActiveRenderer())
		{
			RenderTextUtils::LoadDefaultFont(RendererManager::Instance.ActiveRenderer(), "DefaultFont.ttf");
		};
#endif

		for(;;)
		{
			UpdateSubsystems();

			while(GameClockMayPerformFixedTimeStep())
			{
				OnFixedUpdate();
			};

			OnFrameUpdate();

			if(ShouldQuit())
				break;

			if(FirstFrame)
			{
				FirstFrame = false;

#if USE_GRAPHICS
				if(IsGUISandbox)
				{
#	if !FLPLATFORM_ANDROID
					if(!FileSystemWatcher::Instance.WatchDirectory(FileSystemUtils::ResourcesDirectory()))
					{
						Instance.Dispose();

						DeInitSubsystems();

						return 1;
					};

					FileSystemWatcher::Instance.OnAction.Connect<GameInterface, &GameInterface::OnGUISandboxTrigger>(this);
#	endif

					ReloadGUI();
				};
#endif
			};

#if USE_GRAPHICS
			if(IsMobile && !RendererManager::Instance.Input.HasFocus)
				sf::sleep(sf::milliseconds(1000));
#endif
		};

		if(DeInitFunction)
		{
			try
			{
				ProtectedLuaCast<bool>(DeInitFunction());
			}
			catch(std::exception &e)
			{
				Instance.Dispose();

				DeInitSubsystems();

				return 1;
			};
		};

#if USE_GRAPHICS
		RendererManager::Instance.Input.ClearContexts();
#endif

		Instance.Dispose();

		DeInitSubsystems();

		return 0;
	};

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
	void ScriptedGameInterface::OnFrameBegin(Renderer *TheRenderer)
	{
		if(ErroredOnFrameBegin)
			return;

		STATIC_FUNCTION_CHECK_RETURN_VOID(OnFrameBeginFunction);

		try
		{
			OnFrameBeginFunction(TheRenderer);
		}
		catch(std::exception &e)
		{
			ErroredOnFrameBegin = true;
		};
	};

	void ScriptedGameInterface::OnFrameDraw(Renderer *TheRenderer)
	{
		if(ErroredOnFrameDraw)
			return;

		STATIC_FUNCTION_CHECK_RETURN_VOID(OnFrameDrawFunction);

		try
		{
			OnFrameDrawFunction(TheRenderer);
		}
		catch(std::exception &e)
		{
			ErroredOnFrameDraw = true;
		};
	};

	void ScriptedGameInterface::OnFrameEnd(Renderer *TheRenderer)
	{
		if(ErroredOnFrameEnd)
			return;

		STATIC_FUNCTION_CHECK_RETURN_VOID(OnFrameEndFunction);

		try
		{
			OnFrameEndFunction(TheRenderer);
		}
		catch(std::exception &e)
		{
			ErroredOnFrameEnd = true;
		};

		GameInterface::OnFrameEnd(TheRenderer);
	};

	void ScriptedGameInterface::OnResize(Renderer *TheRenderer, uint32 Width, uint32 Height)
	{
		if(ErroredOnResize)
			return;

		STATIC_FUNCTION_CHECK_RETURN_VOID(OnResizeFunction);

		try
		{
			OnResizeFunction(TheRenderer, Width, Height);
		}
		catch(std::exception &e)
		{
			ErroredOnResize = true;
		};
	};

	void ScriptedGameInterface::OnResourcesReloaded(Renderer *TheRenderer)
	{
		if(ErroredOnResourcesReloaded)
			return;

		STATIC_FUNCTION_CHECK_RETURN_VOID(OnResourcesReloadedFunction);

		try
		{
			OnResourcesReloadedFunction(TheRenderer);
		}
		catch(std::exception &e)
		{
			ErroredOnResourcesReloaded = true;
		};
	};
#endif

	void ScriptedGameInterface::OnFixedUpdate()
	{
		if(ErroredOnFixedUpdate)
			return;

		STATIC_FUNCTION_CHECK_RETURN_VOID(OnFixedUpdateFunction);

		try
		{
			OnFixedUpdateFunction();
		}
		catch(std::exception &e)
		{
			ErroredOnFixedUpdate = true;
		};
	};

	void ScriptedGameInterface::OnFrameUpdate()
	{
		if(ErroredOnFrameUpdate)
			return;

		STATIC_FUNCTION_CHECK_RETURN_VOID(OnFrameUpdateFunction);

		try
		{
			OnFrameUpdateFunction();
		}
		catch(std::exception &e)
		{
			ErroredOnFrameUpdate = true;
		};
	};

	bool ScriptedGameInterface::ShouldQuit()
	{
		STATIC_FUNCTION_CHECK_RETURN_BOOL(ShouldQuitFunction, true);

		try
		{
			return ProtectedLuaCast<bool>(ShouldQuitFunction()) || QuitFlag;
		}
		catch(std::exception &e)
		{
			return true;
		};
	};
};
