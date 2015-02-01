#include "FlamingCore.hpp"

void register_freetype_font_renderer();

namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define PROFILER_FONT_SIZE 14
#	define TAG "RendererManager"

	Vector4 Renderer::DefaultClearColor(1, 1, 1, 1);

	RendererHandle Counter = 0, Active = 0;
	RendererManager RendererManager::Instance;
	bool FirstRenderer = true;

	DisposablePointer<IRendererImplementation> DefaultImpl;

	class RendererInputProcessor : public InputCenter::Context
	{
	public:
		uint32 CurrentCommand;

		RendererInputProcessor() : CurrentCommand(0)
		{
		}

		bool OnKey(const InputCenter::KeyInfo &Key) override
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

					case InputKey::Down:
						if (Console::Instance.CommandLog.size())
						{
							CurrentCommand++;

							if(CurrentCommand > Console::Instance.CommandLog.size())
							{
								CurrentCommand = 0;
							}

							if (CurrentCommand == Console::Instance.CommandLog.size())
							{
								TheManager.ConsoleCursorPosition = 0;
								TheManager.ConsoleText = "";
							}
							else
							{
								TheManager.ConsoleCursorPosition = Console::Instance.CommandLog[CurrentCommand].length();
								TheManager.ConsoleText = Console::Instance.CommandLog[CurrentCommand];
							}
						}

						break;

					case InputKey::Up:
						if (Console::Instance.CommandLog.size())
						{
							if (CurrentCommand == 0)
							{
								CurrentCommand = Console::Instance.CommandLog.size();
							}
							else
							{
								CurrentCommand--;
							}

							if (CurrentCommand == Console::Instance.CommandLog.size())
							{
								TheManager.ConsoleCursorPosition = 0;
								TheManager.ConsoleText = "";
							}
							else
							{
								TheManager.ConsoleCursorPosition = Console::Instance.CommandLog[CurrentCommand].length();
								TheManager.ConsoleText = Console::Instance.CommandLog[CurrentCommand];
							}
						}

						break;

					case InputKey::BackSpace:
						if(TheManager.ConsoleCursorPosition > 0)
						{
							std::string ActualConsoleText = TheManager.ConsoleText;

							TheManager.ConsoleText = ActualConsoleText.substr(0, TheManager.ConsoleCursorPosition - 1) + ActualConsoleText.substr(TheManager.ConsoleCursorPosition);

							TheManager.ConsoleCursorPosition--;
						}

						break;

					case InputKey::Return:
						if(TheManager.ConsoleText.length())
						{
							Console::Instance.RunConsoleCommand(TheManager.ConsoleText);

							CurrentCommand = Console::Instance.CommandLog.size();

							TheManager.ConsoleText.clear();
							TheManager.ConsoleCursorPosition = 0;
						}

						break;
					}

					const InputCenter::Action *const ToggleConsoleAction = TheManager.Input.GetAction(MakeStringID("TOGGLECONSOLE"));

					if(TheManager.ShowConsole && ToggleConsoleAction && ToggleConsoleAction->Type == InputActionType::Keyboard &&
						Key.Name == ToggleConsoleAction->Index)
					{
						return false;
					}
				}

				return true;
			}

			return false;
		}

		bool OnMouseButton(const InputCenter::MouseButtonInfo &Button) override
		{
			RendererManager &TheManager = RendererManager::Instance;

			return TheManager.ShowConsole;
		}

		bool OnJoystickButton(const InputCenter::JoystickButtonInfo &Button) override
		{
			RendererManager &TheManager = RendererManager::Instance;

			return TheManager.ShowConsole;
		}
		
		bool OnJoystickAxis(const InputCenter::JoystickAxisInfo &Axis) override
		{
			RendererManager &TheManager = RendererManager::Instance;

			return TheManager.ShowConsole;
		}

		bool OnTouch(const InputCenter::TouchInfo &Touch) override
		{
			RendererManager &TheManager = RendererManager::Instance;

			return TheManager.ShowConsole;
		}

		void OnJoystickConnected(uint8 Index) override {}

		void OnJoystickDisconnected(uint8 Index) override {}

		void OnMouseMove(const Vector2 &Position) override {}

		void OnCharacterEntered(wchar_t Character) override
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
			}
		}

		void OnAction(const InputCenter::Action &TheAction) override
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
				}
			}
		}

		void OnGainFocus() override {}
		void OnLoseFocus() override {}
	};

	RendererHandle Renderer::Handle() const
	{
		return HandleValue;
	}

	Vector2 Renderer::Size() const
	{
		return Impl->Size();
	}

	void Renderer::RenderLines(const Vector3 &p1, const Vector3 &p2, const Vector3 &p3, const Vector3 &p4, uint32 Steps, const Vector4 &Color)
	{
		static std::vector<VertexColorVertex> Geometry;
		Geometry.resize(0);

		f32 tStep = 1 / (f32)Steps;

		VertexColorVertex Vertex;
		Vertex.Color = Color;

		for (f32 t = 0; t <= 1; t += tStep)
		{
			Vertex.Position = MathUtils::BezierInterpolate4(p1, p2, p3, p4, t);
			Geometry.push_back(Vertex);

			if (t > 0 && t != 1)
				Geometry.push_back(Vertex);
		}

		if (!IsVertexBufferHandleValid(LineBuffer))
		{
			LineBuffer = CreateVertexBuffer();
		}

		if (!IsVertexBufferHandleValid(LineBuffer))
			return;

		SpriteCache::Instance.Flush(this);

		BindTexture((TextureHandle)0);

		SetVertexBufferData(LineBuffer, VertexDetailsMode::Mixed, VertexColorFormat, sizeof(VertexColorFormat) / sizeof(VertexColorFormat[0]), &Geometry[0], Geometry.size() * sizeof(VertexColorVertex));

		RenderVertices(VertexModes::Lines, LineBuffer, 0, Geometry.size());
	}

	RendererHandle RendererManager::AddRenderer(const char *Title, uint32 Width, uint32 Height, uint32 Style, RendererCapabilities Caps)
	{
		FLASSERT(&Instance == this, "We're not our own instance!");

		//Add a renderer
		RendererHandle Out = ++Counter;
		DisposablePointer<Renderer> *Info = &Renderers[Out];
		Info->Reset(new Renderer(new DEFAULT_RENDERER_IMPLEMENTATION()));
		Info->Get()->HandleValue = Out;
		Info->Get()->UIRoot.Reset(new UIRootWidget());

		Log::Instance.LogInfo(TAG, "Attempting to create a Renderer ('%s'; (%dx%d))", Title, Width, Height);

		if(!Info->Get()->Create(Title, Width, Height, Style, Caps))
		{
			Info->Dispose();

			return 0;
		}

		Info->Get()->UIRoot->SetRect(TBRect(0, 0, Info->Get()->Size().x, Info->Get()->Size().y));

		return Out;
	}

	RendererHandle RendererManager::AddRenderer(void *Window, RendererCapabilities Caps)
	{
		FLASSERT(&Instance == this, "We're not our own instance!");

		//Add a renderer
		RendererHandle Out = ++Counter;
		DisposablePointer<Renderer> *Info = &Renderers[Out];
		Info->Reset(new Renderer(new DEFAULT_RENDERER_IMPLEMENTATION()));
		Info->Get()->HandleValue = Out;
		Info->Get()->UIRoot.Reset(new UIRootWidget());

		Log::Instance.LogInfo(TAG, "Attempting to create a Renderer (WND: 0x%08x)", StringUtils::PointerString(Window).c_str());

		if(!Info->Get()->Create(Window, Caps))
		{
			Info->Dispose();

			return 0;
		}

		Info->Get()->UIRoot->SetRect(TBRect(0, 0, Info->Get()->Size().x, Info->Get()->Size().y));

		return Out;
	}

	void RendererManager::DestroyRenderer(RendererHandle Handle)
	{
		FLASSERT(&Instance == this, "We're not our own instance!");

		Renderers.erase(Handle);
	}

	void RendererManager::SetActiveRenderer(RendererHandle Handle)
	{
		FLASSERT(&Instance == this, "We're not our own instance!");
		FLASSERT(Renderers.find(Handle) != Renderers.end(), "Renderer not found");

		Active = Handle;

		DisposablePointer<Renderer> &In = Renderers[Handle];
		//TODO
		//In->Window.setActive();
	}

	uint32 RendererManager::RendererCount() const
	{
		return Renderers.size();
	}

	DisposablePointer<Renderer> RendererManager::ActiveRenderer()
	{
		FLASSERT(&Instance == this, "We're not our own instance!");
		FLASSERT(Renderers.find(Active) != Renderers.end(), "Renderer not found");

		DisposablePointer<Renderer> Out = Renderers[Active];
		Out.MakeReadOnly();

		return Out;
	}

#if PROFILER_ENABLED
	void RendererManager::OnGetProfilerPackets(const Profiler::PacketMap &Packets)
	{
		ProfilerPackets = Packets;
	}
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
			}

			if(!Input.Update(it->second.Get()))
			{
				ReturnValue = false;
			}
		}

		if(!Input.HasFocus && PlatformInfo::PlatformType == PlatformType::Mobile)
			return ReturnValue;

		Renderer *Renderer = ActiveRenderer();

		for (uint32 i = 0; i <= Renderer->ScenePasses.size(); i++)
		{
			if (i < Renderer->ScenePasses.size())
			{
				Renderer->ScenePasses[i]->Bind(Renderer);
			}

			{
				PROFILE("Render FrameStart", StatTypes::Rendering);
				Renderer->OnFrameStarted(Renderer, i == Renderer->ScenePasses.size() ? SCENEPASS_END : Renderer->ScenePasses[i]->GetName());
			}

			{
				PROFILE("Render FrameDraw", StatTypes::Rendering);
				Renderer->OnFrameDraw(Renderer, i == Renderer->ScenePasses.size() ? SCENEPASS_END : Renderer->ScenePasses[i]->GetName());
			}

			{
				PROFILE("Render FrameEnd", StatTypes::Rendering);
				Renderer->OnFrameEnded(Renderer, i == Renderer->ScenePasses.size() ? SCENEPASS_END : Renderer->ScenePasses[i]->GetName());
			}

			if (i < Renderer->ScenePasses.size())
			{
				Renderer->ScenePasses[i]->Unbind(Renderer);
			}
		}

		UI->Owner = Renderer;

		TBAnimationManager::Update();
		Renderer->UIRoot->InvokeProcessStates();
		Renderer->UIRoot->InvokeProcess();

		UI->BeginPaint(Renderer->Size().x, Renderer->Size().y);
		Renderer->UIRoot->InvokePaint(TBWidget::PaintProps());
		UI->EndPaint();

		if (TBAnimationManager::HasAnimationsRunning())
			Renderer->UIRoot->Invalidate();

		TBMessageHandler::ProcessMessages();

		//TODO: Optimize this to prevent doing it twice per frame on dev mode?
		bool PushOrtho = Renderer->MatrixStackSize() == 0;

		if(PushOrtho)
		{
			Rect ScreenRect(PlatformInfo::RotateScreen(Rect(0, Renderer->Size().x, Renderer->Size().y, 0)));

			Renderer->PushMatrices();
			Renderer->SetProjectionMatrix(Matrix4x4::OrthoMatrixRH(ScreenRect.Left, ScreenRect.Right, ScreenRect.Bottom, ScreenRect.Top, -1, 1));
			Renderer->SetWorldMatrix(Matrix4x4());
			Renderer->SetViewport(0, 0, Renderer->Size().x, Renderer->Size().y);
		}

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
				}

				RenderTextUtils::RenderText(Renderer, str.str(), TextParams().FontSize(PROFILER_FONT_SIZE)
					.Color(TextColor).BorderColor(Vector4(0, 0, 0, 1)).BorderSize(1).Position(Vector2(0, (f32)YPos)));

				str << std::dec;
			}

			{
				Vector4 TextColor;

				if(PercentLeft == 0)
				{
					TextColor = Vector4(0, 1, 1, 1);
				}
				else
				{
					TextColor = Vector4(1, 1, 0, 1);
				}

				str.str("");
				str << "UFT: " << std::fixed << PercentLeft << "% (" << std::fixed << TimeThisFrame * PercentLeft / 100.f << " ms)";

				RenderTextUtils::RenderText(Renderer, str.str(), TextParams().FontSize(PROFILER_FONT_SIZE)
					.Color(TextColor).BorderColor(Vector4(0, 0, 0, 1)).BorderSize(1).Position(Vector2(0, (f32)YPos)));

				str << std::dec;
			}
		}
#endif

		if(ShowConsole)
		{
			Sprite TheSprite;
			TheSprite.Options.Scale(Renderer->Size()).Color(Vector4(0, 0, 0, 0.9f));
			TheSprite.Draw(Renderer);

			std::string ActualConsoleText = ConsoleText;
			ActualConsoleText = ActualConsoleText.substr(0, ConsoleCursorPosition) + "|" + ActualConsoleText.substr(ConsoleCursorPosition);
			TextParams Params;
			Params.BorderColor(Vector4(0, 0, 0, 1)).BorderSize(1).Position(Vector2(0, Renderer->Size().y - PROFILER_FONT_SIZE * 1.3f)).FontSize(PROFILER_FONT_SIZE);

			RenderTextUtils::RenderText(Renderer, ActualConsoleText, Params);

			Params.PositionValue.y -= PROFILER_FONT_SIZE;

			for(int32 i = Console::Instance.ConsoleLog.size() - 1; i >= 0; i--, Params.PositionValue.y -= PROFILER_FONT_SIZE)
			{
				RenderTextUtils::RenderText(Renderer, Console::Instance.ConsoleLog[i], Params);
			}
		}

		if(PushOrtho)
		{
			Renderer->PopMatrices();
		}

		Renderer->Display();

		return ReturnValue;
	}

	Renderer::Renderer(IRendererImplementation *_Impl) : Impl(_Impl), HandleValue(0), LineBuffer(0)
	{
		FLASSERT(Impl, "Invalid Implementation!");

		if(!Impl)
			return;

		Impl->Target = this;
	}

	void Renderer::ScreenResizedTransforms(Rect &Viewport, Matrix4x4 &Projection, Matrix4x4 &World)
	{
		f32 TargetAspectRatio = BaseResolutionValue.x / BaseResolutionValue.y;

		Vector2 RealSizeValue(Size());
		Vector2 SizeValue(RealSizeValue.x, RealSizeValue.x / TargetAspectRatio + 0.5f);

		if (SizeValue.y > RealSizeValue.y)
		{
			SizeValue = Vector2(RealSizeValue.y * TargetAspectRatio + 0.5f, RealSizeValue.y);
		}

		Viewport.Left = MathUtils::Round((RealSizeValue.x - SizeValue.x) / 2.f);
		Viewport.Top = MathUtils::Round((RealSizeValue.y - SizeValue.y) / 2.f);
		Viewport.Right = Viewport.Left + SizeValue.x;
		Viewport.Bottom = Viewport.Top + SizeValue.y;

		Projection = Matrix4x4::OrthoMatrixRH(0, RealSizeValue.x, RealSizeValue.y, 0, -1, 1);

		Vector2 Scale(RealSizeValue.x / BaseResolutionValue.x, RealSizeValue.y / BaseResolutionValue.y);
		
		World = Matrix4x4::Scale(Vector4(Scale, 1, 1));
	}

	bool Renderer::Create(void *WindowHandle, RendererCapabilities ExpectedCaps)
	{
		bool Result = Impl->Create(WindowHandle, ExpectedCaps);

		if(Result)
		{
			BaseResolutionValue = Size();
		}

		return Result;
	}

	bool Renderer::Create(const std::string &Title, uint32 Width, uint32 Height, uint32 Style, RendererCapabilities ExpectedCaps)
	{
		bool Result = false;

		Rect ScreenRect(0, (f32)Width, 0, (f32)Height);

		if (PlatformInfo::ResolutionOverrideWidth > 0)
		{
			ScreenRect.Right = (f32)PlatformInfo::ResolutionOverrideWidth;
			ScreenRect.Bottom = (f32)PlatformInfo::ResolutionOverrideHeight;
		}

		ScreenRect = PlatformInfo::RotateScreen(ScreenRect);

		Vector2 ScreenSize = PlatformInfo::ScreenSize(ScreenRect);

		Result = Impl->Create(Title, (uint32)ScreenSize.x, (uint32)ScreenSize.y, Style, ExpectedCaps);

		if(Result)
		{
			BaseResolutionValue = Size();
		}

		return Result;
	}

	void Renderer::AddScenePass(const std::string &Name)
	{
		for (uint32 i = 0; i < ScenePasses.size(); i++)
		{
			if (ScenePasses[i]->Name == Name)
			{
				Log::Instance.LogWarn(TAG, "Unable to add scene pass '%s': Already found", Name.c_str());

				return;
			}
		}

		DisposablePointer<ScenePass> Out(new ScenePass());
		Out->Name = Name;

		ScenePasses.push_back(Out);
	}

	void Renderer::RemoveScenePass(const std::string &Name)
	{
		for (uint32 i = 0; i < ScenePasses.size(); i++)
		{
			if (ScenePasses[i]->Name == Name)
			{
				ScenePasses[i].Dispose();

				ScenePasses.erase(ScenePasses.begin() + i);

				return;
			}
		}
	}

	VertexBufferHandle Renderer::CreateVertexBuffer()
	{
		return Impl->CreateVertexBuffer();
	}

	void Renderer::SetVertexBufferData(VertexBufferHandle Handle, uint8 DetailsMode, VertexElementDescriptor *Elements, uint32 ElementCount, const void *Data, uint32 DataByteSize)
	{
		Impl->SetVertexBufferData(Handle, DetailsMode, Elements, ElementCount, Data, DataByteSize);
	}

	bool Renderer::IsVertexBufferHandleValid(VertexBufferHandle Handle)
	{
		return Impl->IsTextureHandleValid(Handle);
	}

	void Renderer::DestroyVertexBuffer(VertexBufferHandle Handle)
	{
		SpriteCache::Instance.Flush(this);
		Impl->DestroyVertexBuffer(Handle);
	}

	FrameBufferHandle Renderer::CreateFrameBuffer(const FrameBufferCreationInfo &Info)
	{
		return Impl->CreateFrameBuffer(Info);
	}

	bool Renderer::IsFrameBufferValid(FrameBufferHandle Handle)
	{
		return Impl->IsFrameBufferValid(Handle);
	}

	void Renderer::BindFrameBuffer(FrameBufferHandle Handle)
	{
		Impl->BindFrameBuffer(Handle);
	}

	void Renderer::DestroyFrameBuffer(FrameBufferHandle Handle)
	{
		Impl->DestroyFrameBuffer(Handle);
	}

	void Renderer::RenderVertices(uint32 VertexMode, VertexBufferHandle Buffer, uint32 Start, uint32 End)
	{
		Impl->RenderVertices(VertexMode, Buffer, Start, End);
	}

	void Renderer::Display()
	{
		SpriteCache::Instance.Flush(this);

		Impl->Display();
	}

	const RendererFrameStats &Renderer::FrameStats() const
	{
		return Impl->FrameStats();
	}

	void Renderer::SetMousePosition(const Vector2 &Position)
	{
		Impl->SetMousePosition(Position);
	}

	void Renderer::SetClipRect(const Rect &_ClippingRect)
	{
		SpriteCache::Instance.Flush(this);

		Impl->SetClipRect(_ClippingRect);
	}

	void Renderer::Clear(uint32 ID)
	{
		Impl->Clear(ID);
	}

	void Renderer::BindTexture(TextureHandle Handle)
	{
		Impl->BindTexture(Handle);
	}

	void Renderer::BindTexture(Texture *t)
	{
		Impl->BindTexture(t == NULL ? 0 : t->Handle());
	}

	bool Renderer::GetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 BufferByteCount)
	{
		return Impl->GetTextureData(Handle, Pixels, BufferByteCount);
	}

	void Renderer::SetBlendingMode(uint32 BlendingMode)
	{
		Impl->SetBlendingMode(BlendingMode);
	}

	void Renderer::SetWorldMatrix(const Matrix4x4 &WorldMatrix)
	{
		SpriteCache::Instance.Flush(this);

		LastWorldMatrix = WorldMatrix;

		Impl->SetWorldMatrix(WorldMatrix);
	}

	void Renderer::SetProjectionMatrix(const Matrix4x4 &ProjectionMatrix)
	{
		SpriteCache::Instance.Flush(this);

		LastProjectionMatrix = ProjectionMatrix;

		Impl->SetProjectionMatrix(ProjectionMatrix);
	}

	void Renderer::PushMatrices()
	{
		SpriteCache::Instance.Flush(this);

		MatrixStackElement Element;
		Element.World = LastWorldMatrix;
		Element.Projection = LastProjectionMatrix;
		Element.Viewport = LastViewport;

		MatrixStack.push_back(Element);
	}

	void Renderer::PopMatrices()
	{
		if(MatrixStack.size() == 0)
			return;

		SpriteCache::Instance.Flush(this);

		const MatrixStackElement &Element = MatrixStack.back();

		SetWorldMatrix(Element.World);
		SetProjectionMatrix(Element.Projection);
		SetViewport(Element.Viewport.Left, Element.Viewport.Top, Element.Viewport.Right - Element.Viewport.Left, Element.Viewport.Bottom - Element.Viewport.Top);

		MatrixStack.pop_back();
	}

	const Matrix4x4 &Renderer::WorldMatrix()
	{
		return LastWorldMatrix;
	}

	const Matrix4x4 &Renderer::ProjectionMatrix()
	{
		return LastProjectionMatrix;
	}

	void Renderer::SetViewport(f32 x, f32 y, f32 Width, f32 Height)
	{
		LastViewport = Rect(x, x + Width, y, y + Height);

		SpriteCache::Instance.Flush(this);

		Impl->SetViewport(x, y, Width, Height);
	}

	TextureHandle Renderer::CreateTexture()
	{
		return Impl->CreateTexture();
	}

	bool Renderer::IsTextureHandleValid(TextureHandle Handle)
	{
		return Impl->IsTextureHandleValid(Handle);
	}

	void Renderer::DestroyTexture(TextureHandle Handle)
	{
		SpriteCache::Instance.Flush(this);

		Impl->DestroyTexture(Handle);
	}

	void Renderer::SetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 Width, uint32 Height)
	{
		SpriteCache::Instance.Flush(this);

		Impl->SetTextureData(Handle, Pixels, Width, Height);
	}

	void Renderer::SetTextureWrapMode(TextureHandle Handle, uint32 WrapMode)
	{
		SpriteCache::Instance.Flush(this);

		Impl->SetTextureWrapMode(Handle, WrapMode);
	}

	void Renderer::SetTextureFiltering(TextureHandle Handle, uint32 Filtering)
	{
		SpriteCache::Instance.Flush(this);

		Impl->SetTextureFiltering(Handle, Filtering);
	}

	bool Renderer::CaptureScreen(uint8 *Pixels, uint32 BufferByteCount)
	{
		SpriteCache::Instance.Flush(this);

		return Impl->CaptureScreen(Pixels, BufferByteCount);
	}

	bool Renderer::PollEvent(RendererEvent &Out)
	{
		return Impl->PollEvent(Out);
	}

	void *Renderer::WindowHandle() const
	{
		return Impl->WindowHandle();
	}

	void Renderer::SetFrameRate(uint32 FPS)
	{
		Impl->SetFrameRate(FPS);
	}

	void Renderer::ReportSkippedDrawCall()
	{
		Impl->ReportSkippedDrawCall();
	}

	const RendererCapabilities &Renderer::Capabilities() const
	{
		return Impl->Capabilities();
	}

	RendererDisplayMode Renderer::DesktopDisplayMode()
	{
		return DefaultImpl->DesktopDisplayMode();
	}

	std::vector<RendererDisplayMode> Renderer::DisplayModes()
	{
		return DefaultImpl->DisplayModes();
	}

	uint32 Renderer::MatrixStackSize() const
	{
		return MatrixStack.size();
	}

	const Vector2 &Renderer::BaseResolution() const
	{
		//Almost 100% sure this is Portrait... Maybe set this up as Mobile only?
		/*
#if FLPLATFORM_MOBILE
		if(Size().x < Size().y)
			return Vector2(BaseResolutionValue.y, BaseResolutionValue.x);
#endif
		*/

		return BaseResolutionValue;
	}

	Vector2 Renderer::ScaleCoordinate(const Vector2 &Coordinate) const
	{
		//KISS till find a better solution...
		Vector2 ScaleFactor = Size() / BaseResolution();

		return ScaleFactor * Coordinate;
	}

	void RendererManager::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK()

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo(TAG, "Initializing Renderer Manager...");

		Input.Initialize();

		DisposablePointer<RendererInputProcessor> TheInputProcessor(new RendererInputProcessor);
		TheInputProcessor->Name = "RENDERER INPUT";

		Input.AddContext(TheInputProcessor);
		Input.EnableContext(MakeStringID(TheInputProcessor->Name), true);

		DefaultImpl.Reset(new DEFAULT_RENDERER_IMPLEMENTATION());

#if PROFILER_ENABLED
		Profiler::Instance.OnFinishFrame.Connect<RendererManager, &RendererManager::OnGetProfilerPackets>(this);
#endif

		UI.Reset(new UIRenderer());

		if (!tb_core_init(UI.Get(), "/UIResources/language/lng_en.tb.txt"))
		{
			Log::Instance.LogErr(TAG, "Failed to initialize TurboBadger, UI is not available!");
		}

		TBWidgetsAnimationManager::Init();

		if (!g_tb_skin->Load("/UIResources/default_skin/skin.tb.txt"))
		{
			Log::Instance.LogErr(TAG, "Failed to load the TurboBadger Skin, UI is not available!");
		}

		register_freetype_font_renderer();

		g_font_manager->AddFontInfo("/DefaultFont.ttf", "DefaultFont");

		// Set the default font description for widgets to one of the fonts we just added
		TBFontDescription fd;
		fd.SetID(TBIDC("DefaultFont"));
		fd.SetSize(g_tb_skin->GetDimensionConverter()->DpToPx(14));
		g_font_manager->SetDefaultFontDescription(fd);

		// Create the font now.
		TBFontFace *font = g_font_manager->CreateFontFace(g_font_manager->GetDefaultFontDescription());

		UIInput.Reset(new UIInputProcessor());
		UIInput->Name = "GUIPROCESSOR";

		RendererManager::Instance.Input.AddContext(UIInput);
		RendererManager::Instance.Input.EnableContext(MakeStringID(UIInput->Name));
	}

	void RendererManager::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		SubSystem::Shutdown(Priority);

		Log::Instance.LogInfo(TAG, "Terminating Renderer Manager...");

		while(Renderers.begin() != Renderers.end())
		{
			Renderers.begin()->second.Dispose();
			Renderers.erase(Renderers.begin());
		}

		DefaultImpl.Dispose();

#if PROFILER_ENABLED
		Profiler::Instance.OnFinishFrame.Disconnect<RendererManager, &RendererManager::OnGetProfilerPackets>(this);
#endif

		Input.DisableContext(MakeStringID("GUIPROCESSOR"));

		TBWidgetsAnimationManager::Shutdown();

		tb_core_shutdown();
	}

	void RendererManager::Update(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		SubSystem::Update(Priority);
	}

	RendererManager::RendererManager() : SubSystem(RENDERERMANAGER_PRIORITY), ShowProfiler(false), ShowConsole(false),
		ConsoleCursorPosition(0), ConsoleLogOffset(0) {}

	ScenePass::ScenePass() : FrameBuffer(INVALID_FTGHANDLE)
	{
	}

	ScenePass::~ScenePass()
	{
	}

	void ScenePass::Bind(Renderer *TheRenderer)
	{
		TheRenderer->BindFrameBuffer(FrameBuffer);
	}

	void ScenePass::Unbind(Renderer *TheRenderer)
	{
		TheRenderer->BindFrameBuffer(INVALID_FTGHANDLE);
	}

	void ScenePass::Draw(Renderer *TheRenderer)
	{
	}

#	endif
}
