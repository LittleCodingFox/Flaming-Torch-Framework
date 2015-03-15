#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	define TAG "Main"

	DisposablePointer<GameInterface> g_Game;

	bool GameInterface::LoadPackage(const std::string &PackageName)
	{
		DisposablePointer<Stream> TheStream(new FileStream());

		if(!TheStream.AsDerived<FileStream>()->Open(PackageName, StreamFlags::Read))
		{
			g_Log.LogErr(TAG, "Unable to open '%s' for reading!", PackageName.c_str());

			return false;
		}

		if(!g_PackageManager.AddPackage(MakeStringID(PackageName), TheStream))
		{
			g_Log.LogErr(TAG, "Unable to load the '%s' Package!", PackageName.c_str());

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
					g_Log.LogWarn(TAG, "Unknown rotation ID '%c'", c);
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

		g_Log.Register();
		g_PackageManager.Register();

		InitSubsystems();

		if (!LoadPackage(FileSystemUtils::ResourcesDirectory() + "/Content/Default.package"))
		{
			g_Log.LogErr(TAG, "Failed to perform basic initialization, quitting...");

			g_Game.Dispose();

			DeInitSubsystems();

			return 1;
		}

		g_Clock.Register();
		g_FPSCounter.Register();
		g_ResourceManager.Register();
		g_Console.Register();
		g_Future.Register();
		g_Profiler.Register();
		g_LuaScript.Register();

#if USE_SOUND
		g_Sound.Register();
#endif

#if !FLPLATFORM_MOBILE
		g_FileWatcher.Register();
#endif

#if USE_GRAPHICS
		g_Input.Register();
		g_Renderer.Register();
#endif

		InitSubsystems();

		return 0;
	}

	void GameInterface::OnFixedUpdate()
	{
	}

	void GameInterface::OnFrameUpdate()
	{
	}

#if USE_GRAPHICS
#	if !FLPLATFORM_ANDROID
	void GameInterface::OnGUISandboxTrigger(const std::string &Directory, const std::string &FileName, uint32 Action)
	{
		if(FileName == "GUILayout.resource")
			ReloadGUI();
	}
#	endif

	void GameInterface::ReloadGUI()
	{
		g_Renderer.UI->ClearLayouts();

		DisposablePointer<FileStream> InputStream(new FileStream());

		if (!InputStream->Open(FileSystemUtils::ResourcesDirectory() + "GUILayout.resource", StreamFlags::Read | StreamFlags::Text) ||
			!g_Renderer.UI->LoadLayouts(InputStream))
		{
			g_Log.LogErr(TAG, "Failed to reload our GUI Layouts!");

			return;
		}
	}

	bool GameInterface::CreateRenderer(const RenderCreateOptions &Options)
	{
		if(Options.WindowHandle != NULL)
		{
			g_Log.LogInfo(TAG, "Creating renderer from window handle '0x%08x'", Options.WindowHandle);

			if (!g_Renderer.Create(Options.WindowHandle, Options.Caps))
			{
				g_Log.LogErr(TAG, "Failed to create renderer");

				return false;
			}
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

			g_Log.LogInfo(TAG, str.str().c_str());

			if (!g_Renderer.Create(Options.Title.c_str(), Options.Width, Options.Height, Options.Style, Options.Caps))
			{
				g_Log.LogErr(TAG, "Failed to create renderer");

				return false;
			}
		}

		if (BaseResolution != Vector2())
		{
			g_Renderer.BaseResolutionValue = BaseResolution;
		}
		else
		{
			g_Renderer.BaseResolutionValue = g_Renderer.Size();
		}

		g_Renderer.OnFrameStarted.Connect<GameInterface, &GameInterface::OnFrameBegin>(this);
		g_Renderer.OnFrameDraw.Connect<GameInterface, &GameInterface::OnFrameDraw>(this);
		g_Renderer.OnFrameEnded.Connect<GameInterface, &GameInterface::OnFrameEnd>(this);
		g_Renderer.OnResized.Connect<GameInterface, &GameInterface::OnResize>(this);
		g_Renderer.OnResourcesReloaded.Connect<GameInterface, &GameInterface::OnResourcesReloaded>(this);

		if(Options.FrameRate != 0)
		{
			g_Renderer.SetFrameRate(Options.FrameRate);
		}

		Vector2 RendererSize = g_Renderer.Size();

		g_Renderer.SetViewport(0, 0, RendererSize.x, RendererSize.y);

		return true;
	}
#endif

	void GameInterfaceSingleFrame()
	{
		UpdateSubsystems();

		while (GameClockMayPerformFixedTimeStep())
		{
			g_Game->OnFixedUpdate();
		}

		g_Game->OnFrameUpdate();
	}

	int32 NativeGameInterface::Run(int32 argc, char **argv)
	{
		int32 Result = GameInterface::Run(argc, argv);

		if (Result != 0)
			return Result;

		if(!Initialize(argc, argv))
		{
			g_Log.LogErr(TAG, "Failed to perform basic initialization, quitting...");

			g_Game.Dispose();

			DeInitSubsystems();

			return 1;
		}

		DisposablePointer<Stream> AutoExecStream(new FileStream());

		if(!AutoExecStream.AsDerived<FileStream>()->Open(FileSystemUtils::PreferredStorageDirectory() + "/autoexec.cfg", StreamFlags::Read | StreamFlags::Text))
		{
			AutoExecStream = g_PackageManager.GetFile(MakeStringID("/"), MakeStringID("autoexec.cfg"));
		}

		if(AutoExecStream)
		{
			std::vector<std::string> Commands = StringUtils::Split(StringUtils::Strip(AutoExecStream->AsString(), '\r'), '\n');

			for(uint32 i = 0; i < Commands.size(); i++)
			{
				if(Commands[i].length() && Commands[i][0] != '#')
				{
					g_Console.RunConsoleCommand(Commands[i]);
				}
			}
		}

		GameClockSetFixedFrameRate(FixedUpdateRate());

#if USE_GRAPHICS
		RenderTextUtils::LoadDefaultFont("DefaultFont.ttf");

		if (IsGUISandbox)
		{
#	if !FLPLATFORM_MOBILE
			if (!g_FileWatcher.WatchDirectory(FileSystemUtils::ResourcesDirectory()))
			{
				g_Game.Dispose();

				DeInitSubsystems();

				return 1;
			}

			g_FileWatcher.OnAction.Connect<GameInterface, &GameInterface::OnGUISandboxTrigger>(this);
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
			if (PlatformInfo::PlatformType == PlatformType::Mobile && !g_Input.HasFocus)
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
#endif
		}
#endif

		if(!DeInitialize())
		{
			g_Log.LogErr(TAG, "Failed to properly deinitialize!");

			g_Game.Dispose();

			DeInitSubsystems();

			return 1;
		}

		g_Log.LogInfo(TAG, "Finished");

		g_Game.Dispose();

		DeInitSubsystems();

		return 0;
	}
}
