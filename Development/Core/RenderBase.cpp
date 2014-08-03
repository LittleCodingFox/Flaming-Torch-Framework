#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define PROFILER_FONT_SIZE 14

	RendererHandle Counter = 0, Active = 0;
	RendererManager RendererManager::Instance;
	bool FirstRenderer = true;

	class RendererInputProcessor : public InputCenter::Context
	{
	public:
		bool OnKey(const InputCenter::KeyInfo &Key)
		{
			RendererManager &TheManager = RendererManager::Instance;

			if(TheManager.ShowConsole)
			{
				if(Key.JustPressed)
				{
					switch(Key.Name)
					{
					case sf::Keyboard::Left:
						if(TheManager.ConsoleCursorPosition > 0)
							TheManager.ConsoleCursorPosition--;

						break;

					case sf::Keyboard::Right:
						if(TheManager.ConsoleCursorPosition < TheManager.ConsoleText.getSize())
							TheManager.ConsoleCursorPosition++;

						break;

					case sf::Keyboard::BackSpace:
						if(TheManager.ConsoleCursorPosition > 0)
						{
							std::wstring ActualConsoleText = TheManager.ConsoleText.toWideString();

							TheManager.ConsoleText = ActualConsoleText.substr(0, TheManager.ConsoleCursorPosition - 1) + ActualConsoleText.substr(TheManager.ConsoleCursorPosition);

							TheManager.ConsoleCursorPosition--;
						};

						break;

					case sf::Keyboard::Return:
						if(TheManager.ConsoleText.getSize())
						{
							Console::Instance.RunConsoleCommand(TheManager.ConsoleText);

							TheManager.ConsoleText.clear();
							TheManager.ConsoleCursorPosition = 0;
						};

						break;
					};

					const InputCenter::Action *const ToggleConsoleAction = TheManager.Input.GetAction(MakeStringID("TOGGLECONSOLE"));

					if(TheManager.ShowConsole && ToggleConsoleAction && ToggleConsoleAction->Type == InputActionType::Keyboard &&
						Key.Name == ToggleConsoleAction->Index)
					{
						return false;
					};
				};

				return true;
			};

			return false;
		};

		bool OnMouseButton(const InputCenter::MouseButtonInfo &Button)
		{
			RendererManager &TheManager = RendererManager::Instance;

			return TheManager.ShowConsole;
		};

		bool OnJoystickButton(const InputCenter::JoystickButtonInfo &Button)
		{
			RendererManager &TheManager = RendererManager::Instance;

			return TheManager.ShowConsole;
		};
		
		bool OnJoystickAxis(const InputCenter::JoystickAxisInfo &Axis)
		{
			RendererManager &TheManager = RendererManager::Instance;

			return TheManager.ShowConsole;
		};

		void OnJoystickConnected(uint8 Index) {};

		void OnJoystickDisconnected(uint8 Index) {};

		void OnMouseMove(const Vector3 &Position) {};

		void OnCharacterEntered(wchar_t Character)
		{
			RendererManager &TheManager = RendererManager::Instance;

			if(TheManager.ShowConsole)
			{
				if(Character == 8 || //Backspace
					Character == L'\r' //Return
					)
					return;

				std::wstring ActualConsoleText = TheManager.ConsoleText.toWideString();

				wchar_t String[2] = {
					Character, L'\0'
				};

				TheManager.ConsoleText = ActualConsoleText.substr(0, TheManager.ConsoleCursorPosition) +
					std::wstring(String) + ActualConsoleText.substr(TheManager.ConsoleCursorPosition);

				TheManager.ConsoleCursorPosition++;
			};
		};

		void OnAction(const InputCenter::Action &TheAction)
		{
			RendererManager &TheManager = RendererManager::Instance;

			if(TheAction.Type == InputActionType::Keyboard && TheAction.Key()->JustPressed)
			{
				if(TheAction.Name == "TOGGLEPROFILER")
				{
					TheManager.ShowProfiler = !TheManager.ShowProfiler;
				}
				else if(TheAction.Name == "TOGGLECONSOLE")
				{
					TheManager.ShowConsole = !TheManager.ShowConsole;
				};
			};
		};

		void OnGainFocus() {};
		void OnLoseFocus() {};
	};

	void DumpRendererStats()
	{
		Log::Instance.LogInfo("RendererManager", "GL Info");
		Log::Instance.LogInfo("RendererManager", "   Renderer: %s", glGetString(GL_RENDERER));
		Log::Instance.LogInfo("RendererManager", "   Vendor: %s", glGetString(GL_VENDOR));
		Log::Instance.LogInfo("RendererManager", "   Version: %s", glGetString(GL_VERSION));

		int t, t2, t3;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &t);

		GLCHECK();

		Log::Instance.LogInfo("RendererManager", "   Max Texture Size: %d", t);

		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &t);
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &t2);
		glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &t3);

		GLCHECK();

		Log::Instance.LogInfo("RendererManager", "   Max Texture Units: %d (%d vs, %d ps)", t, t3, t2);

		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &t);

		GLCHECK();

		Log::Instance.LogInfo("RendererManager", "   Max Vertex Attribs: %d", t);
	};

	bool RendererManager::Renderer::GetBoolFeature(uint32 ID)
	{
		switch(ID)
		{
		case RendererFeature::GlewIsAvailable:
			return GlewIsAvailable;

		case RendererFeature::AnisotropicSupported:
			return AnisotropicSupported;

		case RendererFeature::AnisotropicEnabled:
			return AnisotropicEnabled;

		case RendererFeature::CanAutoGenerateMipMaps:
			return CanAutoGenerateMipMaps;

		case RendererFeature::MayUseNonPowerOfTwoTextures:
			return MayUseNonPOTTextures;

		default:
			FLASSERT(0, "Invalid ID for GetBoolFeature: '%d'", ID);
		};

		return false;
	};

	f32 RendererManager::Renderer::GetFloatFeature(uint32 ID)
	{
		switch(ID)
		{
		case RendererFeature::MaxAnisotropicLevel:
			return MaxAnisotropicLevel;

		case RendererFeature::CurrentAnisotropicLevel:
			return CurrentAnisotropicLevel;

		default:
			FLASSERT(0, "Invalid ID for GetFloatFeature: '%d'", ID);
		};

		return 0;
	};

	uint32 RendererManager::Renderer::GetUintFeature(uint32 ID)
	{
		FLASSERT(0, "Unimplemented!");

		return 0;
	};

	RendererHandle RendererManager::Renderer::Handle() const
	{
		return HandleValue;
	};

	Vector2 RendererManager::Renderer::Size() const
	{
		return Vector2((f32)Window.getSize().x, (f32)Window.getSize().y);
	};
	
	bool RendererManager::Renderer::SetFullScreen(const Vector2 &WindowSize, bool Value)
	{
		if(CreatedFromWindow)
			return false;

		Matrix4x4 Projection, World;

		glGetFloatv(GL_PROJECTION_MATRIX, &Projection.m[0][0]);
		glGetFloatv(GL_MODELVIEW_MATRIX, &World.m[0][0]);

		ResourceManager::Instance.PrepareResourceReload();

		Window.create(sf::VideoMode((uint32)WindowSize.x, (uint32)WindowSize.y), WindowTitle,
			Value ? WindowStyle | sf::Style::Fullscreen : WindowStyle & ~sf::Style::Fullscreen);

		if(!Window.isOpen())
			return false;

		if(GlewIsAvailable) {
			AnisotropicSupported = !!glewIsSupported("GL_EXT_texture_filter_anisotropic");

			if(AnisotropicSupported)
			{
				AnisotropicEnabled = true;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &MaxAnisotropicLevel);
				CurrentAnisotropicLevel = MaxAnisotropicLevel;
			};

			CanAutoGenerateMipMaps = !!glewIsSupported("GL_ARB_framebuffer_object");
			MayUseNonPOTTextures = !!glewIsSupported("GL_ARB_texture_non_power_of_two");

			GLCHECK();
		};

		for(GLStatesMap::iterator it = GLStates.begin(); it != GLStates.end(); it++)
		{
			switch(it->first)
			{
			case GL_VERTEX_ARRAY:
			case GL_TEXTURE_COORD_ARRAY:
			case GL_NORMAL_ARRAY:
			case GL_COLOR_ARRAY:
			case GL_EDGE_FLAG_ARRAY:
			case GL_FOG_COORD_ARRAY:
			case GL_INDEX_ARRAY:
			case GL_SECONDARY_COLOR_ARRAY:
				if(it->second)
				{
					glEnableClientState(it->first);
				}
				else
				{
					glDisableClientState(it->first);
				};

				break;
			default:
				if(it->second)
				{
					glEnable(it->first);
				}
				else
				{
					glDisable(it->first);
				};

				break;
			};
		};

		ResourceManager::Instance.ReloadResources();

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(&Projection.m[0][0]);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&World.m[0][0]);

		FullScreenFlag = Value;

		OnResourcesReloaded(this);

		return true;
	};

	bool RendererManager::Renderer::GetFullScreen() const
	{
		return FullScreenFlag;
	};

	RendererHandle RendererManager::AddRenderer(const char *Title, uint32 Width, uint32 Height, uint32 Style)
	{
		FLASSERT(&Instance == this, "We're not our own instance!");

		//Add a renderer
		RendererHandle Out = ++Counter;
		SuperSmartPointer<Renderer> *Info = &Renderers[Out];
		Info->Reset(new Renderer());
		Info->Get()->HandleValue = Out;

		Log::Instance.LogInfo("RendererManager", "Attempting to create a Renderer ('%s'; (%dx%d))", Title, Width, Height);

		//Create the window
		Info->Get()->Window.create(sf::VideoMode(Width, Height), Title, Style);

		GLCHECK();

		//If it failed to open, return an error
		if(!Info->Get()->Window.isOpen())
		{
			Log::Instance.LogInfo("RendererManager", "Unable to create the rendering window.");
			Renderers.erase(Out);

			return 0;
		};

		//If the first renderer, init glew
		if(FirstRenderer)
		{
			FirstRenderer = false;

			GLenum err = glewInit();

			if(GLEW_OK != err)
			{
				Log::Instance.LogInfo("RendererManager", "GLEW failed to start, so no fancy OpenGL extensions will be available. Error Message: %s",
					glewGetErrorString(err));
				Info->Get()->GlewIsAvailable = false;
			};

			GLCHECK();

			DumpRendererStats();
		};

		//Get some specs
		if(Info->Get()->GlewIsAvailable) {
			Info->Get()->AnisotropicSupported = !!glewIsSupported("GL_EXT_texture_filter_anisotropic");

			if(Info->Get()->AnisotropicSupported)
			{
				Info->Get()->AnisotropicEnabled = true;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &Info->Get()->MaxAnisotropicLevel);
				Info->Get()->CurrentAnisotropicLevel = Info->Get()->MaxAnisotropicLevel;
			};

			Info->Get()->CanAutoGenerateMipMaps = !!glewIsSupported("GL_ARB_framebuffer_object");
			Info->Get()->MayUseNonPOTTextures = !!glewIsSupported("GL_ARB_texture_non_power_of_two");

			GLCHECK();
		};

		Info->Get()->EnableState(GL_TEXTURE_2D);
		Info->Get()->FullScreenFlag = (Style & sf::Style::Fullscreen) != 0;
		Info->Get()->WindowStyle = Style;
		Info->Get()->WindowTitle = Title;
		Info->Get()->CreatedFromWindow = false;
		Info->Get()->UI.Reset(new UIManager(Info->Get()));

		GLCHECK();

		return Out;
	};

	RendererHandle RendererManager::AddRenderer(sf::WindowHandle Window)
	{
		FLASSERT(&Instance == this, "We're not our own instance!");

		//Add a renderer
		RendererHandle Out = ++Counter;
		SuperSmartPointer<Renderer> *Info = &Renderers[Out];
		Info->Reset(new Renderer());
		Info->Get()->HandleValue = Out;

		Log::Instance.LogInfo("RendererManager", "Attempting to create a Renderer (WND: 0x%08x)", Window);

		//Init from an existing window
		Info->Get()->Window.create(Window);

		GLCHECK();

		//If failed to open, return an error
		if(!Info->Get()->Window.isOpen())
		{
			Log::Instance.LogInfo("RendererManager", "Unable to create the rendering window.");
			Renderers.erase(Out);

			return 0;
		};

		//If the first renderer, init glew
		if(FirstRenderer)
		{
			FirstRenderer = false;

			GLenum err = glewInit();

			if(GLEW_OK != err)
			{
				Log::Instance.LogInfo("RendererManager", "GLEW failed to start, so no fancy OpenGL extensions will be available. Error Message: %s",
					glewGetErrorString(err));
				Info->Get()->GlewIsAvailable = false;
			};

			GLCHECK();

			DumpRendererStats();
		};

		//Get some specs
		if(Info->Get()->GlewIsAvailable) {
			Info->Get()->AnisotropicSupported = !!glewIsSupported("GL_EXT_texture_filter_anisotropic");

			if(Info->Get()->AnisotropicSupported)
			{
				Info->Get()->AnisotropicEnabled = true;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &Info->Get()->MaxAnisotropicLevel);
				Info->Get()->CurrentAnisotropicLevel = Info->Get()->MaxAnisotropicLevel;
			};

			Info->Get()->CanAutoGenerateMipMaps = !!glewIsSupported("GL_ARB_framebuffer_object");
			Info->Get()->MayUseNonPOTTextures = !!glewIsSupported("GL_ARB_texture_non_power_of_two");

			GLCHECK();
		};

		Info->Get()->EnableState(GL_TEXTURE_2D);
		Info->Get()->FullScreenFlag = false;
		Info->Get()->CreatedFromWindow = true;
		Info->Get()->UI.Reset(new UIManager(Info->Get()));

		GLCHECK();

		return Out;
	};

	void RendererManager::Renderer::HandleResize(uint32 w, uint32 h)
	{
		sf::View TheView(sf::FloatRect(0, 0, (float)w, (float)h));
		Window.setView(TheView);
		glViewport(0, 0, w, h);
		
		OnResized(this, w, h);

		GLCHECK();
	};

	void RendererManager::DestroyRenderer(RendererHandle Handle)
	{
		FLASSERT(&Instance == this, "We're not our own instance!");

		Renderers.erase(Handle);
	};

	void RendererManager::SetActiveRenderer(RendererHandle Handle)
	{
		FLASSERT(&Instance == this, "We're not our own instance!");
		FLASSERT(Renderers.find(Handle) != Renderers.end(), "Renderer not found");

		Active = Handle;

		SuperSmartPointer<Renderer> &In = Renderers[Handle];
		In->Window.setActive();
	};

	RendererManager::Renderer *RendererManager::ActiveRenderer()
	{
		FLASSERT(&Instance == this, "We're not our own instance!");
		FLASSERT(Renderers.find(Active) != Renderers.end(), "Renderer not found");

		return Renderers[Active].Get();
	};

#if PROFILER_ENABLED
	void RendererManager::OnGetProfilerPackets(const Profiler::PacketMap &Packets)
	{
		ProfilerPackets = Packets;
	};
#endif

	bool RendererManager::RequestFrame()
	{
		bool ReturnValue = true;

		for(RendererMap::iterator it = Renderers.begin(); it != Renderers.end(); it++)
		{
			while(it->second.Get() == NULL)
			{
				Renderers.erase(it);

				it = Renderers.begin();

				if(it == Renderers.end())
					return ReturnValue;
			};

			if(!Input.Update(it->second.Get()))
			{
				ReturnValue = false;
			};
		};

		Renderer *Renderer = ActiveRenderer();

		{
			PROFILE("Render FrameStart", StatTypes::Rendering);
			Renderer->OnFrameStarted(Renderer);
		};

		{
			PROFILE("Render FrameDraw", StatTypes::Rendering);
			Renderer->OnFrameDraw(Renderer);
		};

		{
			PROFILE("Update UI", StatTypes::Rendering);
			Renderer->UI->Update();
		};

		{
			PROFILE("Render UI", StatTypes::Rendering);
			Renderer->UI->Draw(Renderer);
		};

		{
			PROFILE("Render FrameEnd", StatTypes::Rendering);
			Renderer->OnFrameEnded(Renderer);
		};

		SpriteCache::Instance.Flush(Renderer);

#if PROFILER_ENABLED
		if(ShowProfiler)
		{
			static std::stringstream str;
			str.precision(2);

			Renderer->BindTexture(NULL);
			Renderer->EnableState(GL_VERTEX_ARRAY);
			Renderer->DisableState(GL_COLOR_ARRAY);
			Renderer->DisableState(GL_NORMAL_ARRAY);

			Vector2 Vertices[6] = {
				Vector2(),
				Vector2(0, Renderer->Size().y),
				Renderer->Size(),
				Renderer->Size(),
				Vector2(Renderer->Size().x, 0),
				Vector2(),
			};

			glColor4f(0.45f, 0.45f, 0.45f, 0.5f);

			glVertexPointer(2, GL_FLOAT, 0, Vertices);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			glColor4f(1, 1, 1, 1);

			uint32 YPos = 0;
			uint64 TimeThisFrame = 1000;

			f32 PercentLeft = 100.f;

			for(Profiler::PacketMap::iterator it = ProfilerPackets.begin(); it != ProfilerPackets.end(); it++,
				YPos += PROFILER_FONT_SIZE)
			{
				uint32 Average = (uint32)it->second.ms / it->second.Count;
				f32 PercentThisFrame = it->second.FrameMS / (f32)TimeThisFrame * 100.f;
				PercentLeft -= PercentThisFrame;

				str.str("");
				str << "[" << Profiler::StatTypeString(it->second.Type) << "] " << it->first << " " << it->second.ms << " (" << Average << " avg) - " <<
					std::fixed << PercentThisFrame << "% FT (" << std::fixed << TimeThisFrame * PercentThisFrame / 100.f << "ms)";

				sf::Color TextColor;

				if(PercentThisFrame < 5.f)
				{
					TextColor = sf::Color::Cyan;
				}
				else if(PercentThisFrame < 10.f)
				{
					TextColor = sf::Color::Yellow;
				}
				else
				{
					TextColor = sf::Color::Red;
				};

				RenderTextUtils::RenderText(Renderer, str.str(), TextParams().FontSize(PROFILER_FONT_SIZE)
					.Color(Vector4(TextColor.r / 255.f, TextColor.g / 255.f, TextColor.b / 255.f, TextColor.a / 255.f))
					.BorderColor(Vector4(0, 0, 0, 1)).BorderSize(1).Position(Vector2(0, (f32)YPos)));
			};

			{
				sf::Color TextColor;

				if(PercentLeft == 0)
				{
					TextColor = sf::Color::Cyan;
				}
				else
				{
					TextColor = sf::Color::Yellow;
				};

				str.str("");
				str << "UFT: " << std::fixed << PercentLeft << "% (" << std::fixed << TimeThisFrame * PercentLeft / 100.f << " ms)";

				RenderTextUtils::RenderText(Renderer, str.str(), TextParams().FontSize(PROFILER_FONT_SIZE)
					.Color(Vector4(TextColor.r / 255.f, TextColor.g / 255.f, TextColor.b / 255.f, TextColor.a / 255.f))
					.BorderColor(Vector4(0, 0, 0, 1)).BorderSize(1).Position(Vector2(0, (f32)YPos)));
			};
		};
#endif

		if(ShowConsole)
		{
			Sprite TheSprite;
			TheSprite.Options.Scale(Renderer->Size()).Color(Vector4(0, 0, 0, 0.9f));
			TheSprite.Draw(Renderer);

			std::wstring ActualConsoleText = ConsoleText.toWideString();
			ActualConsoleText = ActualConsoleText.substr(0, ConsoleCursorPosition) + L"|" + ActualConsoleText.substr(ConsoleCursorPosition);
			TextParams Params;
			Params.Color(Renderer->UI->GetDefaultFontColor()).SecondaryColor(Renderer->UI->GetDefaultSecondaryFontColor())
				.BorderColor(Vector4(0, 0, 0, 1)).BorderSize(1).Position(Vector2(0, Renderer->Size().y - PROFILER_FONT_SIZE * 1.3f)).FontSize(PROFILER_FONT_SIZE);

			RenderTextUtils::RenderText(Renderer, ActualConsoleText, Params);

			Params.PositionValue.y -= PROFILER_FONT_SIZE;

			for(int32 i = Console::Instance.ConsoleLog.size() - 1; i >= 0; i--, Params.PositionValue.y -= PROFILER_FONT_SIZE)
			{
				RenderTextUtils::RenderText(Renderer, Console::Instance.ConsoleLog[i], Params);
			};
		};

		Renderer->Window.display();

		return ReturnValue;
	};

	void RendererManager::Renderer::Clear(uint32 ID)
	{
		glClear(ID);
	};

	bool RendererManager::Renderer::IsStateEnabled(uint32 ID) const
	{
		GLStatesMap::const_iterator it = GLStates.find(ID);

		if(it == GLStates.end())
		{
			if(ID == GL_DITHER || ID == GL_MULTISAMPLE)
				return true;

			return false;
		};

		return it->second;
	};

	void RendererManager::Renderer::EnableState(uint32 ID)
	{
		if(IsStateEnabled(ID))
			return;

		switch(ID)
		{
		case GL_VERTEX_ARRAY:
		case GL_TEXTURE_COORD_ARRAY:
		case GL_NORMAL_ARRAY:
		case GL_COLOR_ARRAY:
		case GL_EDGE_FLAG_ARRAY:
		case GL_FOG_COORD_ARRAY:
		case GL_INDEX_ARRAY:
		case GL_SECONDARY_COLOR_ARRAY:
			glEnableClientState(ID);

			break;
		default:
			glEnable(ID);

			break;
		};
		
		GLStates[ID] = true;
	};

	void RendererManager::Renderer::DisableState(uint32 ID)
	{
		if(!IsStateEnabled(ID))
			return;

		switch(ID)
		{
		case GL_VERTEX_ARRAY:
		case GL_TEXTURE_COORD_ARRAY:
		case GL_NORMAL_ARRAY:
		case GL_COLOR_ARRAY:
		case GL_EDGE_FLAG_ARRAY:
		case GL_FOG_COORD_ARRAY:
		case GL_INDEX_ARRAY:
		case GL_SECONDARY_COLOR_ARRAY:
			glDisableClientState(ID);

			break;
		default:
			glDisable(ID);

			break;
		};

		GLStates[ID] = false;
	};

	void RendererManager::Renderer::BindTexture(Texture *t)
	{
		if(t == LastBoundTexture && t != NULL)
			return;

		LastBoundTexture = t;

		if(t == NULL)
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			t->Bind();
		};
	};

	void RendererManager::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK()

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo("RendererManager", "Initializing Renderer Manager...");

		SuperSmartPointer<RendererInputProcessor> TheInputProcessor(new RendererInputProcessor);
		TheInputProcessor->Name = "RENDERER INPUT";

		Input.AddContext(TheInputProcessor);
		Input.EnableContext(MakeStringID(TheInputProcessor->Name), true);

#if PROFILER_ENABLED
		Profiler::Instance.OnFinishFrame.Connect(this, &RendererManager::OnGetProfilerPackets);
#endif
	};

	void RendererManager::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		SubSystem::Shutdown(Priority);

		Log::Instance.LogInfo("RendererManager", "Terminating Renderer Manager...");

		while(Renderers.begin() != Renderers.end())
		{
			Renderers.begin()->second.Dispose();
			Renderers.erase(Renderers.begin());
		};

#if PROFILER_ENABLED
		Profiler::Instance.OnFinishFrame.Disconnect(this, &RendererManager::OnGetProfilerPackets);
#endif
	};

	void RendererManager::Update(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		SubSystem::Update(Priority);
	};

	RendererManager::RendererManager() : SubSystem(RENDERERMANAGER_PRIORITY), ShowProfiler(false), ShowConsole(false),
		ConsoleCursorPosition(0), ConsoleLogOffset(0) {};

#	endif
};
