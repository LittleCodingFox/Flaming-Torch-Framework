#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define PROFILER_FONT_SIZE 14
#	define TAG "RendererManager"

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
					case InputKey::Left:
						if(TheManager.ConsoleCursorPosition > 0)
							TheManager.ConsoleCursorPosition--;

						break;

					case InputKey::Right:
						if(TheManager.ConsoleCursorPosition < TheManager.ConsoleText.length())
							TheManager.ConsoleCursorPosition++;

						break;

					case InputKey::BackSpace:
						if(TheManager.ConsoleCursorPosition > 0)
						{
							std::string ActualConsoleText = TheManager.ConsoleText;

							TheManager.ConsoleText = ActualConsoleText.substr(0, TheManager.ConsoleCursorPosition - 1) + ActualConsoleText.substr(TheManager.ConsoleCursorPosition);

							TheManager.ConsoleCursorPosition--;
						};

						break;

					case InputKey::Return:
						if(TheManager.ConsoleText.length())
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

				std::string ActualConsoleText = TheManager.ConsoleText;

				char String[2] = {
					(char)Character, '\0'
				};

				TheManager.ConsoleText = ActualConsoleText.substr(0, TheManager.ConsoleCursorPosition) +
					std::string(String) + ActualConsoleText.substr(TheManager.ConsoleCursorPosition);

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

	RendererHandle Renderer::Handle() const
	{
		return HandleValue;
	};

	Vector2 Renderer::Size() const
	{
		FLASSERT(Impl, "Invalid Implementation!");

		return Impl->Size();
	};

	RendererHandle RendererManager::AddRenderer(const char *Title, uint32 Width, uint32 Height, uint32 Style)
	{
		FLASSERT(&Instance == this, "We're not our own instance!");

		//Add a renderer
		RendererHandle Out = ++Counter;
		SuperSmartPointer<Renderer> *Info = &Renderers[Out];
		Info->Reset(new Renderer(new DEFAULT_RENDERER_IMPLEMENTATION()));
		Info->Get()->HandleValue = Out;

		Log::Instance.LogInfo(TAG, "Attempting to create a Renderer ('%s'; (%dx%d))", Title, Width, Height);

		if(!Info->Get()->Create(Title, Width, Height, Style))
			return 0;

		Info->Get()->UI.Reset(new UIManager(Info->Get()));

		return Out;
	};

	RendererHandle RendererManager::AddRenderer(void *Window)
	{
		FLASSERT(&Instance == this, "We're not our own instance!");

		//Add a renderer
		RendererHandle Out = ++Counter;
		SuperSmartPointer<Renderer> *Info = &Renderers[Out];
		Info->Reset(new Renderer(new DEFAULT_RENDERER_IMPLEMENTATION()));
		Info->Get()->HandleValue = Out;

		Log::Instance.LogInfo(TAG, "Attempting to create a Renderer (WND: 0x%08x)", Window);

		Info->Get()->UI.Reset(new UIManager(Info->Get()));

		return Out;
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
		//In->Window.setActive();
	};

	uint32 RendererManager::RendererCount() const
	{
		return Renderers.size();
	};

	Renderer *RendererManager::ActiveRenderer()
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

			static Sprite ProfilerBackground;
			ProfilerBackground.Options.Scale(Renderer->Size()).Color(Vector4(0.45f, 0.45f, 0.45f, 0.5f));

			ProfilerBackground.Draw(Renderer);

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

				Vector4 TextColor;

				if(PercentThisFrame < 5.f)
				{
					TextColor = Vector4(0, 1, 1, 1);
				}
				else if(PercentThisFrame < 10.f)
				{
					TextColor = Vector4(1, 1, 0, 1);
				}
				else
				{
					TextColor = Vector4(1, 0, 0, 1);
				};

				RenderTextUtils::RenderText(Renderer, str.str(), TextParams().FontSize(PROFILER_FONT_SIZE)
					.Color(TextColor).BorderColor(Vector4(0, 0, 0, 1)).BorderSize(1).Position(Vector2(0, (f32)YPos)));
			};

			{
				Vector4 TextColor;

				if(PercentLeft == 0)
				{
					TextColor = Vector4(0, 1, 1, 1);
				}
				else
				{
					TextColor = Vector4(1, 1, 0, 1);
				};

				str.str("");
				str << "UFT: " << std::fixed << PercentLeft << "% (" << std::fixed << TimeThisFrame * PercentLeft / 100.f << " ms)";

				RenderTextUtils::RenderText(Renderer, str.str(), TextParams().FontSize(PROFILER_FONT_SIZE)
					.Color(TextColor).BorderColor(Vector4(0, 0, 0, 1)).BorderSize(1).Position(Vector2(0, (f32)YPos)));
			};
		};
#endif

		if(ShowConsole)
		{
			Sprite TheSprite;
			TheSprite.Options.Scale(Renderer->Size()).Color(Vector4(0, 0, 0, 0.9f));
			TheSprite.Draw(Renderer);

			std::string ActualConsoleText = ConsoleText;
			ActualConsoleText = ActualConsoleText.substr(0, ConsoleCursorPosition) + "|" + ActualConsoleText.substr(ConsoleCursorPosition);
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

		Renderer->Impl->Display();

		return ReturnValue;
	};

	Renderer::Renderer(IRendererImplementation *_Impl) : Impl(_Impl)
	{
		Impl->Target = this;
	};

	bool Renderer::Create(void *WindowHandle)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return false;

		return Impl->Create(WindowHandle);
	};

	bool Renderer::Create(const std::string &Title, uint32 Width, uint32 Height, uint32 Style)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return false;

		return Impl->Create(Title, Width, Height, Style);
	};

	VertexBufferHandle Renderer::CreateVertexBuffer()
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return 0;

		return Impl->CreateVertexBuffer();
	};

	void Renderer::SetVertexBufferData(VertexBufferHandle Handle, uint8 DetailsMode, VertexElementDescriptor *Elements, uint32 ElementCount, const void *Data, uint32 DataByteSize)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return;

		Impl->SetVertexBufferData(Handle, DetailsMode, Elements, ElementCount, Data, DataByteSize);
	};

	bool Renderer::IsVertexBufferHandleValid(VertexBufferHandle Handle)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return 0;

		return Impl->IsTextureHandleValid(Handle);
	};

	void Renderer::DestroyVertexBuffer(VertexBufferHandle Handle)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return;

		Impl->DestroyVertexBuffer(Handle);
	};


	void Renderer::RenderVertices(uint32 VertexMode, VertexBufferHandle Buffer, uint32 Start, uint32 End)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return;

		Impl->RenderVertices(VertexMode, Buffer, Start, End);
	};

	void Renderer::SetMousePosition(const Vector2 &Position)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return;

		Impl->SetMousePosition(Position);
	};

	void Renderer::Clear(uint32 ID)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return;

		Impl->Clear(ID);
	};

	void Renderer::BindTexture(TextureHandle Handle)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return;

		Impl->BindTexture(Handle);
	};

	void Renderer::BindTexture(Texture *t)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return;

		Impl->BindTexture(t == NULL ? 0 : t->Handle());
	};

	bool Renderer::GetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 BufferByteCount)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return false;

		return Impl->GetTextureData(Handle, Pixels, BufferByteCount);
	};

	void Renderer::SetBlendingMode(uint32 BlendingMode)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return;

		Impl->SetBlendingMode(BlendingMode);
	};

	void Renderer::SetWorldMatrix(const Matrix4x4 &WorldMatrix)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return;

		Impl->SetWorldMatrix(WorldMatrix);
	};

	void Renderer::SetProjectionMatrix(const Matrix4x4 &ProjectionMatrix)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return;

		Impl->SetProjectionMatrix(ProjectionMatrix);
	};

	void Renderer::SetViewport(f32 x, f32 y, f32 Width, f32 Height)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return;

		Impl->SetViewport(x, y, Width, Height);
	};

	TextureHandle Renderer::CreateTexture()
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return 0;

		return Impl->CreateTexture();
	};

	bool Renderer::IsTextureHandleValid(TextureHandle Handle)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return false;

		return Impl->IsTextureHandleValid(Handle);
	};

	void Renderer::DestroyTexture(TextureHandle Handle)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return;

		Impl->DestroyTexture(Handle);
	};

	void Renderer::SetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 Width, uint32 Height)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return;

		Impl->SetTextureData(Handle, Pixels, Width, Height);
	};

	void Renderer::SetTextureWrapMode(TextureHandle Handle, uint32 WrapMode)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return;

		Impl->SetTextureWrapMode(Handle, WrapMode);
	};

	void Renderer::SetTextureFiltering(TextureHandle Handle, uint32 Filtering)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return;

		Impl->SetTextureFiltering(Handle, Filtering);
	};

	bool Renderer::CaptureScreen(uint8 *Pixels, uint32 BufferByteCount)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return false;

		return Impl->CaptureScreen(Pixels, BufferByteCount);
	};

	bool Renderer::PollEvent(RendererEvent &Out)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return false;

		return Impl->PollEvent(Out);
	};

	FontHandle Renderer::CreateFont(Stream *Data)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return 0;

		return Impl->CreateFont(Data);
	};

	void Renderer::DestroyFont(FontHandle Handle)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return;

		Impl->DestroyFont(Handle);
	};

	Rect Renderer::MeasureText(FontHandle Handle, const std::string &Text, const TextParams &Parameters)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return Rect();

		return Impl->MeasureText(Handle, Text, Parameters);
	};

	void Renderer::RenderText(FontHandle Handle, const std::string &Text, const TextParams &Parameters)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return;

		Impl->RenderText(Handle, Text, Parameters);
	};

	void *Renderer::WindowHandle() const
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return NULL;

		return Impl->WindowHandle();
	};

	void Renderer::SetFrameRate(uint32 FPS)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return;

		Impl->SetFrameRate(FPS);
	};

	void RendererManager::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK()

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo(TAG, "Initializing Renderer Manager...");

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

		Log::Instance.LogInfo(TAG, "Terminating Renderer Manager...");

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
