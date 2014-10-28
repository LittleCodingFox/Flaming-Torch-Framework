#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define PROFILER_FONT_SIZE 14
#	define TAG "RendererManager"

	RendererHandle Counter = 0, Active = 0;
	RendererManager RendererManager::Instance;
	bool FirstRenderer = true;

	SuperSmartPointer<IRendererImplementation> DefaultImpl;

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

		bool OnTouch(const InputCenter::TouchInfo &Touch)
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
		return Impl->Size();
	};

	RendererHandle RendererManager::AddRenderer(const char *Title, uint32 Width, uint32 Height, uint32 Style, RendererCapabilities Caps)
	{
		FLASSERT(&Instance == this, "We're not our own instance!");

		//Add a renderer
		RendererHandle Out = ++Counter;
		SuperSmartPointer<Renderer> *Info = &Renderers[Out];
		Info->Reset(new Renderer(new DEFAULT_RENDERER_IMPLEMENTATION()));
		Info->Get()->HandleValue = Out;

		Log::Instance.LogInfo(TAG, "Attempting to create a Renderer ('%s'; (%dx%d))", Title, Width, Height);

		if(!Info->Get()->Create(Title, Width, Height, Style, Caps))
		{
			Info->Dispose();

			return 0;
		};

		Info->Get()->UI.Reset(new UIManager(Info->Get()));

		return Out;
	};

	RendererHandle RendererManager::AddRenderer(void *Window, RendererCapabilities Caps)
	{
		FLASSERT(&Instance == this, "We're not our own instance!");

		//Add a renderer
		RendererHandle Out = ++Counter;
		SuperSmartPointer<Renderer> *Info = &Renderers[Out];
		Info->Reset(new Renderer(new DEFAULT_RENDERER_IMPLEMENTATION()));
		Info->Get()->HandleValue = Out;

		Log::Instance.LogInfo(TAG, "Attempting to create a Renderer (WND: 0x%08x)", StringUtils::PointerString(Window).c_str());

		if(!Info->Get()->Create(Window, Caps))
		{
			Info->Dispose();

			return 0;
		};

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
		//TODO
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

		if(!Input.HasFocus)
			return ReturnValue;

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

		//TODO: Optimize this to prevent doing it twice per frame on dev mode?
		bool PushOrtho = Renderer->MatrixStackSize() == 0;

		if(PushOrtho)
		{
			Renderer->PushMatrices();
			Renderer->SetProjectionMatrix(Matrix4x4::OrthoMatrixRH(0, Renderer->Size().x, Renderer->Size().y, 0, -1, 1));
		};

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

				str << std::dec;
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

				str << std::dec;
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

		SpriteCache::Instance.Flush(Renderer);

		if(PushOrtho)
		{
			Renderer->PopMatrices();
		};

		Renderer->Display();

		return ReturnValue;
	};

	Renderer::Renderer(IRendererImplementation *_Impl) : Impl(_Impl), HandleValue(0)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return;

		Impl->Target = this;
	};

	bool Renderer::Create(void *WindowHandle, RendererCapabilities ExpectedCaps)
	{
		return Impl->Create(WindowHandle, ExpectedCaps);
	};

	bool Renderer::Create(const std::string &Title, uint32 Width, uint32 Height, uint32 Style, RendererCapabilities ExpectedCaps)
	{
		return Impl->Create(Title, Width, Height, Style, ExpectedCaps);
	};

	VertexBufferHandle Renderer::CreateVertexBuffer()
	{
		return Impl->CreateVertexBuffer();
	};

	void Renderer::SetVertexBufferData(VertexBufferHandle Handle, uint8 DetailsMode, VertexElementDescriptor *Elements, uint32 ElementCount, const void *Data, uint32 DataByteSize)
	{
		Impl->SetVertexBufferData(Handle, DetailsMode, Elements, ElementCount, Data, DataByteSize);
	};

	bool Renderer::IsVertexBufferHandleValid(VertexBufferHandle Handle)
	{
		return Impl->IsTextureHandleValid(Handle);
	};

	void Renderer::DestroyVertexBuffer(VertexBufferHandle Handle)
	{
		Impl->DestroyVertexBuffer(Handle);
	};

	void Renderer::RenderVertices(uint32 VertexMode, VertexBufferHandle Buffer, uint32 Start, uint32 End)
	{
		Impl->RenderVertices(VertexMode, Buffer, Start, End);
	};

	void Renderer::Display()
	{
		Impl->Display();
	};

	const RendererFrameStats &Renderer::FrameStats() const
	{
		return Impl->FrameStats();
	};

	void Renderer::SetMousePosition(const Vector2 &Position)
	{
		Impl->SetMousePosition(Position);
	};

	void Renderer::StartClipping(const Rect &_ClippingRect)
	{
		FLASSERT(_ClippingRect.Bottom > _ClippingRect.Top, "Expected a larger Bottom coordinate on ClippingRect");
		FLASSERT(_ClippingRect.Right > _ClippingRect.Left, "Expected a larger Right coordinate on ClippingRect");

		if(_ClippingRect.Bottom <= _ClippingRect.Top || _ClippingRect.Right <= _ClippingRect.Left)
			return;

		Rect ClippingRect = _ClippingRect;

		//Clip this rect with the previous rect so that we don't actually clip things that don't make sense
		if(ClippingStack.size())
		{
			const Rect &Parent = ClippingStack.back();

			if(ClippingRect.Left < Parent.Left)
				ClippingRect.Left = Parent.Left;

			if(ClippingRect.Top < Parent.Top)
				ClippingRect.Top = Parent.Top;

			if(ClippingRect.Right > Parent.Right)
				ClippingRect.Right = Parent.Right;

			if(ClippingRect.Bottom > Parent.Bottom)
				ClippingRect.Bottom = Parent.Bottom;
		};

		//Add this even if invalid, since this way we will be able to FinishClipping with no issues of a missing clipping stack frame
		ClippingStack.push_back(ClippingRect);

		//Might be entirely out of the parent area, so check again and skip if necessary
		if(ClippingRect.Bottom <= ClippingRect.Top || ClippingRect.Right <= ClippingRect.Left)
			return;

		SpriteCache::Instance.Flush(this);

		Impl->StartClipping(ClippingRect);
	};

	void Renderer::FinishClipping()
	{
		SpriteCache::Instance.Flush(this);

		if(!ClippingStack.size())
			return;

		ClippingStack.pop_back();

		if(ClippingStack.size())
		{
			const Rect &ClippingRect = ClippingStack.back();

			Impl->StartClipping(ClippingRect);
		}
		else
		{
			Impl->FinishClipping();
		};
	};

	void Renderer::Clear(uint32 ID)
	{
		Impl->Clear(ID);
	};

	void Renderer::BindTexture(TextureHandle Handle)
	{
		Impl->BindTexture(Handle);
	};

	void Renderer::BindTexture(Texture *t)
	{
		Impl->BindTexture(t == NULL ? 0 : t->Handle());
	};

	bool Renderer::GetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 BufferByteCount)
	{
		return Impl->GetTextureData(Handle, Pixels, BufferByteCount);
	};

	void Renderer::SetBlendingMode(uint32 BlendingMode)
	{
		Impl->SetBlendingMode(BlendingMode);
	};

	void Renderer::SetWorldMatrix(const Matrix4x4 &WorldMatrix)
	{
		LastWorldMatrix = WorldMatrix;

		Impl->SetWorldMatrix(WorldMatrix);
	};

	void Renderer::SetProjectionMatrix(const Matrix4x4 &ProjectionMatrix)
	{
		LastProjectionMatrix = ProjectionMatrix;

		Impl->SetProjectionMatrix(ProjectionMatrix);
	};

	void Renderer::PushMatrices()
	{
		MatrixStackElement Element;
		Element.World = LastWorldMatrix;
		Element.Projection = LastProjectionMatrix;

		MatrixStack.push_back(Element);
	};

	void Renderer::PopMatrices()
	{
		if(MatrixStack.size() == 0)
			return;

		const MatrixStackElement &Element = MatrixStack.back();

		SetWorldMatrix(Element.World);
		SetProjectionMatrix(Element.Projection);

		MatrixStack.pop_back();
	};

	const Matrix4x4 &Renderer::WorldMatrix()
	{
		return LastWorldMatrix;
	};

	const Matrix4x4 &Renderer::ProjectionMatrix()
	{
		return LastProjectionMatrix;
	};

	void Renderer::SetViewport(f32 x, f32 y, f32 Width, f32 Height)
	{
		Impl->SetViewport(x, y, Width, Height);
	};

	TextureHandle Renderer::CreateTexture()
	{
		return Impl->CreateTexture();
	};

	bool Renderer::IsTextureHandleValid(TextureHandle Handle)
	{
		return Impl->IsTextureHandleValid(Handle);
	};

	void Renderer::DestroyTexture(TextureHandle Handle)
	{
		Impl->DestroyTexture(Handle);
	};

	void Renderer::SetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 Width, uint32 Height)
	{
		Impl->SetTextureData(Handle, Pixels, Width, Height);
	};

	void Renderer::SetTextureWrapMode(TextureHandle Handle, uint32 WrapMode)
	{
		Impl->SetTextureWrapMode(Handle, WrapMode);
	};

	void Renderer::SetTextureFiltering(TextureHandle Handle, uint32 Filtering)
	{
		Impl->SetTextureFiltering(Handle, Filtering);
	};

	bool Renderer::CaptureScreen(uint8 *Pixels, uint32 BufferByteCount)
	{
		return Impl->CaptureScreen(Pixels, BufferByteCount);
	};

	bool Renderer::PollEvent(RendererEvent &Out)
	{
		return Impl->PollEvent(Out);
	};

	FontHandle Renderer::CreateFont(Stream *Data)
	{
		return Impl->CreateFont(Data);
	};

	void Renderer::DestroyFont(FontHandle Handle)
	{
		Impl->DestroyFont(Handle);
	};

	Rect Renderer::MeasureText(FontHandle Handle, const std::string &Text, const TextParams &Parameters)
	{
		return Impl->MeasureText(Handle, Text, Parameters);
	};

	void Renderer::RenderText(FontHandle Handle, const std::string &Text, const TextParams &Parameters)
	{
		Impl->RenderText(Handle, Text, Parameters);
	};

	void *Renderer::WindowHandle() const
	{
		return Impl->WindowHandle();
	};

	void Renderer::SetFrameRate(uint32 FPS)
	{
		Impl->SetFrameRate(FPS);
	};

	void Renderer::ReportSkippedDrawCall()
	{
		Impl->ReportSkippedDrawCall();
	};

	const RendererCapabilities &Renderer::Capabilities() const
	{
		return Impl->Capabilities();
	};

	RendererDisplayMode Renderer::DesktopDisplayMode()
	{
		return DefaultImpl->DesktopDisplayMode();
	};

	std::vector<RendererDisplayMode> Renderer::DisplayModes()
	{
		return DefaultImpl->DisplayModes();
	};

	uint32 Renderer::MatrixStackSize()
	{
		return MatrixStack.size();
	};

	void RendererManager::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK()

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo(TAG, "Initializing Renderer Manager...");

		Input.Initialize();

		SuperSmartPointer<RendererInputProcessor> TheInputProcessor(new RendererInputProcessor);
		TheInputProcessor->Name = "RENDERER INPUT";

		Input.AddContext(TheInputProcessor);
		Input.EnableContext(MakeStringID(TheInputProcessor->Name), true);

		DefaultImpl.Reset(new DEFAULT_RENDERER_IMPLEMENTATION());

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

		DefaultImpl.Dispose();

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
