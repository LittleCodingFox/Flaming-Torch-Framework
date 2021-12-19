#include "FlamingCore.hpp"

namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define PROFILER_FONT_SIZE 14
#	define TAG "Renderer"

	Renderer g_Renderer;

	Vector4 Renderer::DefaultClearColor(1, 1, 1, 1);

	bool FirstRenderer = true;

	uint8 VertexBufferDataElementSizes[VertexElementDataType::Count] = {
		sizeof(f32),
		sizeof(Vector2),
		sizeof(Vector3),
		sizeof(Vector4)
	};

	uint32 VertexBufferDataElementCount[VertexElementDataType::Count] = {
		1,
		2,
		3,
		4
	};

	DisposablePointer<IRendererImplementation> DefaultImpl;

	class RendererInputProcessor : public Input::Context
	{
	public:
		uint32 CurrentCommand;

		RendererInputProcessor() : CurrentCommand(0)
		{
		}

		bool OnKey(const Input::KeyInfo &Key) override
		{
			if(g_Renderer.ShowConsole)
			{
				if(Key.JustPressed)
				{
					switch(Key.Name)
					{
					case InputKey::Left:
						if(g_Renderer.ConsoleCursorPosition > 0)
							g_Renderer.ConsoleCursorPosition--;

						break;

					case InputKey::Right:
						if(g_Renderer.ConsoleCursorPosition < g_Renderer.ConsoleText.length())
							g_Renderer.ConsoleCursorPosition++;

						break;

					case InputKey::Down:
						if (g_Console.CommandLog.size())
						{
							CurrentCommand++;

							if(CurrentCommand > g_Console.CommandLog.size())
							{
								CurrentCommand = 0;
							}

							if (CurrentCommand == g_Console.CommandLog.size())
							{
								g_Renderer.ConsoleCursorPosition = 0;
								g_Renderer.ConsoleText = "";
							}
							else
							{
								g_Renderer.ConsoleCursorPosition = g_Console.CommandLog[CurrentCommand].length();
								g_Renderer.ConsoleText = g_Console.CommandLog[CurrentCommand];
							}
						}

						break;

					case InputKey::Up:
						if (g_Console.CommandLog.size())
						{
							if (CurrentCommand == 0)
							{
								CurrentCommand = g_Console.CommandLog.size();
							}
							else
							{
								CurrentCommand--;
							}

							if (CurrentCommand == g_Console.CommandLog.size())
							{
								g_Renderer.ConsoleCursorPosition = 0;
								g_Renderer.ConsoleText = "";
							}
							else
							{
								g_Renderer.ConsoleCursorPosition = g_Console.CommandLog[CurrentCommand].length();
								g_Renderer.ConsoleText = g_Console.CommandLog[CurrentCommand];
							}
						}

						break;

					case InputKey::BackSpace:
						if(g_Renderer.ConsoleCursorPosition > 0)
						{
							std::string ActualConsoleText = g_Renderer.ConsoleText;

							g_Renderer.ConsoleText = ActualConsoleText.substr(0, g_Renderer.ConsoleCursorPosition - 1) + ActualConsoleText.substr(g_Renderer.ConsoleCursorPosition);

							g_Renderer.ConsoleCursorPosition--;
						}

						break;

					case InputKey::Return:
						if(g_Renderer.ConsoleText.length())
						{
							g_Console.RunConsoleCommand(g_Renderer.ConsoleText);

							CurrentCommand = g_Console.CommandLog.size();

							g_Renderer.ConsoleText.clear();
							g_Renderer.ConsoleCursorPosition = 0;
						}

						break;
					}

					const Input::Action *const ToggleConsoleAction = g_Input.GetAction(MakeStringID("TOGGLECONSOLE"));

					if(g_Renderer.ShowConsole && ToggleConsoleAction && ToggleConsoleAction->Type == InputActionType::Keyboard &&
						Key.Name == ToggleConsoleAction->Index)
					{
						return false;
					}
				}

				return true;
			}

			return false;
		}

		bool OnMouseButton(const Input::MouseButtonInfo &Button) override
		{
			return g_Renderer.ShowConsole;
		}

		bool OnJoystickButton(const Input::JoystickButtonInfo &Button) override
		{
			return g_Renderer.ShowConsole;
		}
		
		bool OnJoystickAxis(const Input::JoystickAxisInfo &Axis) override
		{
			return g_Renderer.ShowConsole;
		}

		bool OnTouch(const Input::TouchInfo &Touch) override
		{
			return g_Renderer.ShowConsole;
		}

		void OnJoystickConnected(uint8 Index) override {}

		void OnJoystickDisconnected(uint8 Index) override {}

		void OnMouseMove(const Vector2 &Position) override {}

		void OnCharacterEntered(uint32 Character) override
		{
			if(g_Renderer.ShowConsole)
			{
				if(Character == 8 || //Backspace
					Character == L'\r' //Return
					)
					return;

				std::string ActualConsoleText = g_Renderer.ConsoleText;

				char String[2] = {
					(char)Character, '\0'
				};

				g_Renderer.ConsoleText = ActualConsoleText.substr(0, g_Renderer.ConsoleCursorPosition) +
					std::string(String) + ActualConsoleText.substr(g_Renderer.ConsoleCursorPosition);

				g_Renderer.ConsoleCursorPosition++;
			}
		}

		void OnAction(const Input::Action &TheAction) override
		{
			if(TheAction.Type == InputActionType::Keyboard && TheAction.Key()->JustPressed)
			{
				if(TheAction.Name == "TOGGLEPROFILER")
				{
					g_Renderer.ShowProfiler = !g_Renderer.ShowProfiler;
				}
				else if(TheAction.Name == "TOGGLECONSOLE")
				{
					g_Renderer.ShowConsole = !g_Renderer.ShowConsole;
				}
			}
		}

		void OnGainFocus() override {}
		void OnLoseFocus() override {}
	};

	Renderer::Renderer() : SubSystem(RENDERER_PRIORITY), ShowProfiler(false), ShowConsole(false), ConsoleCursorPosition(0), ConsoleLogOffset(0),
		Impl(nullptr), LineBuffer(0), CacheBlendingMode(BlendingMode::Alpha), CacheIndex(0), CacheTexture(NULL), CacheVertexBuffer(INVALID_FTGHANDLE),
		CacheVertexMode(VertexModes::Triangles)
	{
	}

	Vector2 Renderer::Size() const
	{
		if (!WasStarted)
			return Vector2();

		return Impl->Size();
	}

	void Renderer::RenderLines(const Vector3 &p1, const Vector3 &p2, const Vector3 &p3, const Vector3 &p4, uint32 Steps, const Vector4 &Color)
	{
		if (!WasStarted)
			return;

		FlushCachedVertices();

		static std::vector<PosCol> Geometry;
		Geometry.resize(0);

		f32 tStep = 1 / (f32)Steps;

		PosCol Vertex;
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

		BindTexture((TextureHandle)0);

		SetVertexBufferData(LineBuffer, VertexDetailsMode::Mixed, PosColFormat, sizeof(PosColFormat) / sizeof(PosColFormat[0]), &Geometry[0], Geometry.size() * sizeof(PosCol));

		RenderVertices(VertexModes::Lines, LineBuffer, 0, Geometry.size());
	}

	void Renderer::ScreenResizedTransforms(Rect &Viewport, Matrix4x4 &Projection, Matrix4x4 &World)
	{
		if (!WasStarted)
			return;

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
		if (!WasStarted)
			return false;

		bool Result = Impl->Create(WindowHandle, ExpectedCaps);

		if(Result)
		{
			BaseResolutionValue = Size();
			RenderText.resourcesGroup.Reset(new TextureGroup(4096, 4096));
			UI.Dispose();
			UI.Reset(new UIManager());
		}

		return Result;
	}

	bool Renderer::Create(const std::string &Title, uint32 Width, uint32 Height, uint32 Style, RendererCapabilities ExpectedCaps)
	{
		if (!WasStarted)
			return false;

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

			RenderText.resourcesGroup.Reset(new TextureGroup(4096, 4096));
			UI.Dispose();
			UI.Reset(new UIManager());
		}

		return Result;
	}

#if PROFILER_ENABLED
	void Renderer::OnGetProfilerPackets(const Profiler::PacketMap &Packets)
	{
		ProfilerPackets = Packets;
	}
#endif

	VertexBufferHandle Renderer::CreateVertexBuffer()
	{
		if (!WasStarted)
			return INVALID_FTGHANDLE;

		return Impl->CreateVertexBuffer();
	}

	void Renderer::SetVertexBufferData(VertexBufferHandle Handle, uint8 DetailsMode, VertexElementDescriptor *Elements, uint32 ElementCount, const void *Data, uint32 DataByteSize)
	{
		if (!WasStarted)
			return;

		Impl->SetVertexBufferData(Handle, DetailsMode, Elements, ElementCount, Data, DataByteSize);
	}

	bool Renderer::IsVertexBufferHandleValid(VertexBufferHandle Handle)
	{
		if (!WasStarted)
			return false;

		return Impl->IsTextureHandleValid(Handle);
	}

	void Renderer::DestroyVertexBuffer(VertexBufferHandle Handle)
	{
		if (!WasStarted)
			return;

		Impl->DestroyVertexBuffer(Handle);
	}

	void Renderer::RenderVertices(uint32 VertexMode, VertexBufferHandle Buffer, uint32 Start, uint32 End)
	{
		if (!WasStarted)
			return;

		Impl->RenderVertices(VertexMode, Buffer, Start, End);
	}

	void Renderer::RenderCachedVertices(uint32 VertexMode, const void *Data, uint32 DataSize, VertexElementDescriptor *Format, uint32 FormatSize, Texture *TheTexture, uint32 BlendingMode)
	{
		if (!WasStarted)
			return;

		uint32 VertexSize = 0;

		for (uint32 i = 0; i < FormatSize; i++)
		{
			VertexSize = (uint32)MathUtils::Max((f32)VertexSize, (f32)Format[i].Offset + VertexBufferDataElementSizes[Format[i].DataType]);
		}

		FLASSERT(DataSize % VertexSize == 0, "Invalid Vertex Data!");

		if (DataSize % VertexSize != 0)
			return;

		if (BlendingMode != CacheBlendingMode || (TheTexture != CacheTexture && (TheTexture == NULL || CacheTexture == NULL || TheTexture->GetIndex().Index == -1 ||
			TheTexture->GetIndex().Owner.Get() != CacheTexture->GetIndex().Owner.Get())) || FormatSize != CacheVertexFormat.size() || CacheVertexFormat.size() == 0 ||
			memcmp(Format, &CacheVertexFormat[0], sizeof(VertexElementDescriptor) * FormatSize) != 0 || CacheVertexMode != VertexMode || CacheIndex + DataSize > MAX_VERTEX_CACHE_SIZE)
		{
			FlushCachedVertices();
		}

		CacheVertexFormat.resize(FormatSize);
		memcpy(&CacheVertexFormat[0], Format, sizeof(VertexElementDescriptor) * FormatSize);

		CacheBlendingMode = BlendingMode;
		CacheTexture = TheTexture;
		CacheVertexMode = VertexMode;

		memcpy(&CacheBuffer[CacheIndex], Data, DataSize);

		CacheIndex += DataSize;
	}

	void Renderer::FlushCachedVertices()
	{
		if (CacheIndex == 0)
			return;

		BindTexture(CacheTexture);
		//Enable to disable textures
		//BindTexture(TextureHandle(0));

		if (!IsVertexBufferHandleValid(CacheVertexBuffer))
		{
			CacheVertexBuffer = CreateVertexBuffer();
		}

		if (!CacheVertexBuffer)
			return;

		SetVertexBufferData(CacheVertexBuffer, VertexDetailsMode::Mixed, &CacheVertexFormat[0], CacheVertexFormat.size(), &CacheBuffer[0],
			CacheIndex);

		uint32 VertexSize = 0;

		for (uint32 i = 0; i < CacheVertexFormat.size(); i++)
		{
			VertexSize = (uint32)MathUtils::Max((f32)VertexSize, (f32)CacheVertexFormat[i].Offset + VertexBufferDataElementSizes[CacheVertexFormat[i].DataType]);
		}

		FLASSERT(CacheIndex % VertexSize == 0, "Invalid Vertex Data!");

		if (CacheIndex % VertexSize != 0)
		{
			CacheIndex = 0;
			CacheTexture = NULL;

			return;
		}

		uint32 VertexCount = CacheIndex / VertexSize;

		SetBlendingMode(CacheBlendingMode);
		RenderVertices(CacheVertexMode, CacheVertexBuffer, 0, VertexCount);

		CacheIndex = 0;
		CacheTexture = NULL;
	}

	void Renderer::Display()
	{
		if (!WasStarted)
			return;

		FlushCachedVertices();

		Impl->Display();

		RenderText.ClearUnusedResources();
	}

	const RendererFrameStats &Renderer::FrameStats() const
	{
		if (!WasStarted)
		{
			static RendererFrameStats Stats;

			return Stats;
		}

		return Impl->FrameStats();
	}

	void Renderer::SetMousePosition(const Vector2 &Position)
	{
		if (!WasStarted)
			return;

		Impl->SetMousePosition(Position);
	}

	void Renderer::SetClipRect(const Rect &_ClippingRect)
	{
		if (!WasStarted)
			return;

		FlushCachedVertices();

		Impl->SetClipRect(_ClippingRect);
	}

	void Renderer::Clear(uint32 ID)
	{
		if (!WasStarted)
			return;

		Impl->Clear(ID);
	}

	void Renderer::BindTexture(TextureHandle Handle)
	{
		if (!WasStarted)
			return;

		Impl->BindTexture(Handle);
	}

	void Renderer::BindTexture(Texture *t)
	{
		if (!WasStarted)
			return;

		Impl->BindTexture(t == NULL ? 0 : t->Handle());
	}

	bool Renderer::GetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 BufferByteCount)
	{
		if (!WasStarted)
			return false;

		return Impl->GetTextureData(Handle, Pixels, BufferByteCount);
	}

	void Renderer::SetBlendingMode(uint32 BlendingMode)
	{
		if (!WasStarted)
			return;

		if (BlendingMode != CacheBlendingMode)
		{
			FlushCachedVertices();
		}

		Impl->SetBlendingMode(BlendingMode);
	}

	void Renderer::SetWorldMatrix(const Matrix4x4 &WorldMatrix)
	{
		if (!WasStarted)
			return;

		FlushCachedVertices();

		LastWorldMatrix = WorldMatrix;

		Impl->SetWorldMatrix(WorldMatrix);
	}

	void Renderer::SetProjectionMatrix(const Matrix4x4 &ProjectionMatrix)
	{
		if (!WasStarted)
			return;
		
		FlushCachedVertices();

		LastProjectionMatrix = ProjectionMatrix;

		Impl->SetProjectionMatrix(ProjectionMatrix);
	}

	void Renderer::PushMatrices()
	{
		if (!WasStarted)
			return;

		FlushCachedVertices();

		MatrixStackElement Element;
		Element.World = LastWorldMatrix;
		Element.Projection = LastProjectionMatrix;
		Element.Viewport = LastViewport;

		MatrixStack.push_back(Element);
	}

	void Renderer::PopMatrices()
	{
		if (!WasStarted)
			return;

		if (MatrixStack.size() == 0)
			return;

		FlushCachedVertices();

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
		if (!WasStarted)
			return;

		FlushCachedVertices();

		LastViewport = Rect(x, x + Width, y, y + Height);

		Impl->SetViewport(x, y, Width, Height);
	}

	TextureHandle Renderer::CreateTexture()
	{
		if (!WasStarted)
			return INVALID_FTGHANDLE;

		return Impl->CreateTexture();
	}

	bool Renderer::IsTextureHandleValid(TextureHandle Handle)
	{
		if (!WasStarted)
			return false;

		return Impl->IsTextureHandleValid(Handle);
	}

	void Renderer::DestroyTexture(TextureHandle Handle)
	{
		if (!WasStarted)
			return;

		Impl->DestroyTexture(Handle);
	}

	void Renderer::SetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 Width, uint32 Height)
	{
		if (!WasStarted)
			return;

		Impl->SetTextureData(Handle, Pixels, Width, Height);
	}

	void Renderer::SetTextureWrapMode(TextureHandle Handle, uint32 WrapMode)
	{
		if (!WasStarted)
			return;

		Impl->SetTextureWrapMode(Handle, WrapMode);
	}

	void Renderer::SetTextureFiltering(TextureHandle Handle, uint32 Filtering)
	{
		if (!WasStarted)
			return;

		Impl->SetTextureFiltering(Handle, Filtering);
	}

	bool Renderer::CaptureScreen(uint8 *Pixels, uint32 BufferByteCount)
	{
		if (!WasStarted)
			return false;

		return Impl->CaptureScreen(Pixels, BufferByteCount);
	}

	bool Renderer::PollEvent(RendererEvent &Out)
	{
		if (!WasStarted)
			return false;

		return Impl->PollEvent(Out);
	}

	void *Renderer::WindowHandle() const
	{
		if (!WasStarted)
			return nullptr;

		return Impl->WindowHandle();
	}

	void Renderer::SetFrameRate(uint32 FPS)
	{
		if (!WasStarted)
			return;

		Impl->SetFrameRate(FPS);
	}

	void Renderer::ReportSkippedDrawCall()
	{
		if (!WasStarted)
			return;

		Impl->ReportSkippedDrawCall();
	}

	const RendererCapabilities &Renderer::Capabilities() const
	{
		if (!WasStarted)
		{
			static RendererCapabilities Caps;

			return Caps;
		}

		return Impl->Capabilities();
	}

	RendererDisplayMode Renderer::DesktopDisplayMode()
	{
		if (!DefaultImpl.Get())
		{
			static RendererDisplayMode Display;

			return Display;
		}

		return DefaultImpl->DesktopDisplayMode();
	}

	std::vector<RendererDisplayMode> Renderer::DisplayModes()
	{
		if (!DefaultImpl.Get())
			return std::vector<RendererDisplayMode>();

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
		if (!WasStarted)
			return Vector2();

		//KISS till find a better solution...
		Vector2 ScaleFactor = Size() / BaseResolution();

		return ScaleFactor * Coordinate;
	}

	void Renderer::RequestFrame()
	{
		if (!WasStarted)
			return;

		Update(RENDERER_PRIORITY);
	}

	void Renderer::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK()

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		g_Log.LogInfo(TAG, "Initializing Renderer...");

		DisposablePointer<RendererInputProcessor> TheInputProcessor(new RendererInputProcessor);
		TheInputProcessor->Name = "RENDERER INPUT";

		g_Input.AddContext(TheInputProcessor);
		g_Input.EnableContext(MakeStringID(TheInputProcessor->Name), true);

		DefaultImpl.Reset(new DEFAULT_RENDERER_IMPLEMENTATION());
		Impl = new DEFAULT_RENDERER_IMPLEMENTATION();

#if PROFILER_ENABLED
		g_Profiler.OnFinishFrame.Connect<Renderer, &Renderer::OnGetProfilerPackets>(this);
#endif
	}

	void Renderer::Update(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		{
			PROFILE("Render FrameStart", StatTypes::Rendering);
			OnFrameStarted();
		}

		{
			PROFILE("Render FrameDraw", StatTypes::Rendering);
			OnFrameDraw();
		}

		//TODO: Optimize this to prevent doing it twice per frame on dev mode?
		bool PushOrtho = MatrixStackSize() == 0;

		if (PushOrtho)
		{
			Rect ScreenRect(PlatformInfo::RotateScreen(Rect(0, Size().x, Size().y, 0)));

			PushMatrices();
			SetProjectionMatrix(Matrix4x4::OrthoMatrixRH(ScreenRect.Left, ScreenRect.Right, ScreenRect.Bottom, ScreenRect.Top, -1, 1));
			SetWorldMatrix(Matrix4x4());
			SetViewport(0, 0, Size().x, Size().y);
		}

		UI->Draw();
		UI->Update();

		if (PushOrtho)
		{
			PopMatrices();
		}

		{
			PROFILE("Render FrameEnd", StatTypes::Rendering);
			OnFrameEnded();
		}

		//TODO: Optimize this to prevent doing it twice per frame on dev mode?
		PushOrtho = MatrixStackSize() == 0;

		if (PushOrtho)
		{
			Rect ScreenRect(PlatformInfo::RotateScreen(Rect(0, Size().x, Size().y, 0)));

			PushMatrices();
			SetProjectionMatrix(Matrix4x4::OrthoMatrixRH(ScreenRect.Left, ScreenRect.Right, ScreenRect.Bottom, ScreenRect.Top, -1, 1));
			SetWorldMatrix(Matrix4x4());
			SetViewport(0, 0, Size().x, Size().y);
		}

		static std::stringstream str;

		if (!!g_Console.GetVariable("r_drawrenderstats") && g_Console.GetVariable("r_drawrenderstats")->UIntValue != 0)
		{
			str.str("");

			const RendererFrameStats &Stats = FrameStats();

			str << "Renderer: " << Stats.RendererName << " version " << Stats.RendererVersion << " on " << CoreUtils::PlatformString() << "\n" << Stats.RendererCustomMessage << (Stats.RendererCustomMessage.length() ? "\n\n" : "\n");
			str << "Frame Stats:\nDraw calls: " << Stats.DrawCalls << "/" << Stats.DrawCalls + Stats.SkippedDrawCalls << "\nVertex Count: " << Stats.VertexCount << "\nTexture Changes: " << Stats.TextureChanges << "\nMatrix Changes: " << Stats.MatrixChanges <<
				"\nClipping Changes: " << Stats.ClippingChanges << "\nState Changes: " << Stats.StateChanges << "\nActive Resources: " << Stats.TotalResources << " (" << Stats.TotalResourceUsage << " MB)\n";

			RenderTextUtils::RenderText(str.str(), TextParams().fontSize(20).color(Vector4(1, 1, 1, 1))
				.borderColor(Vector4(0, 0, 0, 1)).borderSize(1).position(Vector2(0, 0)));
		}

		if (g_Game.Get() && g_Game->DevelopmentBuild)
		{
			str.str("");

			str << g_FPSCounter.FPS() << " / " << 1000.f / g_FPSCounter.FPS() << "";

			RenderTextUtils::RenderText(str.str(), TextParams().fontSize(10).color(Vector4(1, 1, 1, 1))
				.borderColor(Vector4(0, 0, 0, 1)).borderSize(1).position(Vector2(0, Size().y - 10.0f)));
		}

#if PROFILER_ENABLED
		if (ShowProfiler)
		{
			static std::stringstream str;
			str.precision(2);

			static Sprite ProfilerBackground;
			ProfilerBackground.Options.Scale(Size()).Color(Vector4(0.45f, 0.45f, 0.45f, 0.5f));

			ProfilerBackground.Draw();

			uint32 YPos = 0;
			uint64 TimeThisFrame = 1000;

			f32 PercentLeft = 100.f;

			for (Profiler::PacketMap::iterator it = ProfilerPackets.begin(); it != ProfilerPackets.end(); it++,
				YPos += PROFILER_FONT_SIZE)
			{
				uint32 Average = (uint32)it->second.ms / it->second.Count;
				f32 PercentThisFrame = it->second.FrameMS / (f32)TimeThisFrame * 100.f;
				PercentLeft -= PercentThisFrame;

				str.str("");
				str << "[" << Profiler::StatTypeString(it->second.Type) << "] " << it->first << " " << it->second.ms << " (" << Average << " avg) - " <<
					std::fixed << PercentThisFrame << "% FT (" << std::fixed << TimeThisFrame * PercentThisFrame / 100.f << "ms)";

				Vector4 TextColor;

				if (PercentThisFrame < 5.f)
				{
					TextColor = Vector4(0, 1, 1, 1);
				}
				else if (PercentThisFrame < 10.f)
				{
					TextColor = Vector4(1, 1, 0, 1);
				}
				else
				{
					TextColor = Vector4(1, 0, 0, 1);
				}

				RenderTextUtils::RenderText(str.str(), TextParams().fontSize(PROFILER_FONT_SIZE)
					.color(TextColor).borderColor(Vector4(0, 0, 0, 1)).borderSize(1).position(Vector2(0, (f32)YPos)));

				str << std::dec;
			}

			{
				Vector4 TextColor;

				if (PercentLeft == 0)
				{
					TextColor = Vector4(0, 1, 1, 1);
				}
				else
				{
					TextColor = Vector4(1, 1, 0, 1);
				}

				str.str("");
				str << "UFT: " << std::fixed << PercentLeft << "% (" << std::fixed << TimeThisFrame * PercentLeft / 100.f << " ms)";

				RenderTextUtils::RenderText(str.str(), TextParams().fontSize(PROFILER_FONT_SIZE)
					.color(TextColor).borderColor(Vector4(0, 0, 0, 1)).borderSize(1).position(Vector2(0, (f32)YPos)));

				str << std::dec;
			}
		}
#endif

		if (ShowConsole)
		{
			Sprite TheSprite;
			TheSprite.Options.Scale(Size()).Color(Vector4(0, 0, 0, 0.9f));
			TheSprite.Draw();

			std::string ActualConsoleText = ConsoleText;
			ActualConsoleText = ActualConsoleText.substr(0, ConsoleCursorPosition) + "|" + ActualConsoleText.substr(ConsoleCursorPosition);
			TextParams Params;
			Params.borderColor(Vector4(0, 0, 0, 1)).borderSize(1).position(Vector2(0, Size().y - PROFILER_FONT_SIZE * 1.3f)).fontSize(PROFILER_FONT_SIZE);

			RenderTextUtils::RenderText(ActualConsoleText, Params);

			Params.positionValue.y -= PROFILER_FONT_SIZE;

			for (int32 i = g_Console.ConsoleLog.size() - 1; i >= 0; i--, Params.positionValue.y -= PROFILER_FONT_SIZE)
			{
				RenderTextUtils::RenderText(g_Console.ConsoleLog[i], Params);
			}
		}

		if (PushOrtho)
		{
			PopMatrices();
		}

		Display();
	}

	void Renderer::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		SubSystem::Shutdown(Priority);

		g_Log.LogInfo(TAG, "Terminating Renderer...");

		UI.Dispose();
		DefaultImpl.Dispose();
		delete Impl;

#if PROFILER_ENABLED
		g_Profiler.OnFinishFrame.Disconnect<Renderer, &Renderer::OnGetProfilerPackets>(this);
#endif
	}
#	endif
}
