#include "FlamingCore.hpp"
#if USE_GRAPHICS

namespace tb
{
	using namespace ::FlamingTorch;

	class FTGImageLoader : public TBImageLoader
	{
	public:
		DisposablePointer<TextureBuffer> Buffer;

		virtual int Width() { return Buffer.Get() ? Buffer->Width() : 0; }
		virtual int Height() { return Buffer.Get() ? Buffer->Height() : 0; }
		virtual uint32 *Data() { return (uint32*)&Buffer->Data[0]; }
	};

	TBImageLoader *TBImageLoader::CreateFromFile(const char *Name)
	{
		DisposablePointer<Stream> In = g_PackageManager.GetFile(Path(Name));

		if (!In.Get())
		{
			In.Reset(new FileStream());

			if (!In.AsDerived<FileStream>()->Open(Name, StreamFlags::Read))
			{
				g_Log.LogErr("TBImageLoader", "Failed to load image '%s'", Name);

				return nullptr;
			}
		}

		DisposablePointer<TextureBuffer> Buffer = TextureBuffer::CreateFromStream(In);

		if (!Buffer.Get())
		{
			g_Log.LogErr("TBImageLoader", "Failed to load image '%s'", Name);

			return nullptr;
		}

		FTGImageLoader *Out = new FTGImageLoader();

		Out->Buffer = Buffer;

		return Out;
	}

	void TBSystemCallback(MemoryStream &Stream)
	{
		TBMessageHandler::ProcessMessages();
	}

	void TBSystem::RescheduleTimer(double fire_time)
	{
		if (fire_time == 0)
		{
			g_Future.Post(TBSystemCallback);
		}
		else if (fire_time != TB_NOT_SOON)
		{
			g_Future.PostDelayed(TBSystemCallback, (uint32)(fire_time * 1000));
		}
	}

	class TBStreamFile : public TBFile
	{
	public:
		DisposablePointer<Stream> TheStream;

		virtual long Size()
		{
			return TheStream.Get() ? (long)TheStream->Length() : 0;
		}

		virtual size_t Read(void *buf, size_t elemSize, size_t count)
		{
			if (TheStream->Length() - TheStream->Position() < elemSize * count)
			{
				size_t Remaining = (size_t)(TheStream->Length() - TheStream->Position()) / elemSize;

				if (Remaining == 0)
					return 0;

				return TheStream->Read(buf, elemSize, Remaining) ? Remaining : 0;
			}

			return TheStream->Read(buf, elemSize, count) ? count : 0;
		}
	};

	TBFile *TBFile::Open(const char *Name, TBFileMode Mode)
	{
		Path FilePath(Name), PackagePath(Name[0] == '/' ? Name : std::string("/") + Name);

		DisposablePointer<Stream> In = g_PackageManager.GetFile(PackagePath);

		if (!In)
		{
			In.Reset(new FileStream());

			if (!In.AsDerived<FileStream>()->Open(FilePath.FullPath(), StreamFlags::Read))
				return nullptr;
		}

		TBStreamFile *Out = new TBStreamFile();

		Out->TheStream = In;

		return Out;
	}
}

namespace FlamingTorch
{
	void UIRootWidget::OnInvalid()
	{
		//Do nothing for now
	}

	UIBitmap::~UIBitmap()
	{
		Owner->FlushBitmap(this);
	}

	bool UIBitmap::Init(int width, int height, uint32 *data)
	{
		DisposablePointer<TextureBuffer> Buffer = TextureBuffer::CreateFromData((const uint8 *)data, width, height);

		if (!Buffer.Get())
			return false;

		ContainedTexture = Texture::CreateFromBuffer(Buffer);

		return ContainedTexture;
	}

	int UIBitmap::Width()
	{
		return ContainedTexture.Get() ? ContainedTexture->Width() : 0;
	}

	int UIBitmap::Height()
	{
		return ContainedTexture.Get() ? ContainedTexture->Height() : 0;
	}

	void UIBitmap::SetData(uint32 *data)
	{
		Owner->FlushBitmap(this);

		if (ContainedTexture.Get())
		{
			DisposablePointer<TextureBuffer> Buffer = TextureBuffer::CreateFromData((const uint8 *)data, ContainedTexture->Width(), ContainedTexture->Height());

			if (!Buffer.Get())
				return;

			ContainedTexture = Texture::CreateFromBuffer(Buffer);
		}
	}

	UIRenderer::UIRenderer() : VertexHandle(INVALID_FTGHANDLE)
	{
	}

	void UIRenderer::BeginPaint(int render_target_w, int render_target_h)
	{
		TBRendererBatcher::BeginPaint(render_target_w, render_target_h);

		g_Renderer.PushMatrices();
		g_Renderer.SetProjectionMatrix(Matrix4x4::OrthoMatrixRH(0, (f32)render_target_w, (f32)render_target_h, 0, -1, 1));
		g_Renderer.SetWorldMatrix(Matrix4x4());
		g_Renderer.SetViewport(0, 0, (f32)render_target_w, (f32)render_target_h);
	}

	void UIRenderer::EndPaint()
	{
		g_Renderer.PopMatrices();

		TBRendererBatcher::EndPaint();
	}

	TBBitmap *UIRenderer::CreateBitmap(int width, int height, uint32 *data)
	{
		UIBitmap *Bitmap = new UIBitmap();
		Bitmap->Owner = this;

		if (!Bitmap || !Bitmap->Init(width, height, data))
		{
			delete Bitmap;
			return nullptr;
		}

		return Bitmap;
	}

	VertexElementDescriptor UIFormat[] = {
		{ 0, VertexElementType::Position, VertexElementDataType::Float2 },
		{ sizeof(Vector2), VertexElementType::TexCoord, VertexElementDataType::Float2 },
		{ sizeof(Vector2[2]), VertexElementType::Color, VertexElementDataType::Float4 },
	};

	struct UIVertex
	{
		Vector2 Position, TexCoord;
		Vector4 Color;
	};

	void UIRenderer::RenderBatch(Batch *batch)
	{
		//For Debugging
		/*
		if (!FirstBatch)
			return;

		FirstBatch = false;
		*/

		if (VertexHandle == INVALID_FTGHANDLE)
		{
			VertexHandle = g_Renderer.CreateVertexBuffer();
		}

		static std::vector<UIVertex> VertexData;

		VertexData.resize(batch->vertex_count);

		for (uint32 i = 0; i < VertexData.size(); i++)
		{
			VertexData[i].Position.x = batch->vertex[i].x;
			VertexData[i].Position.y = batch->vertex[i].y;

			VertexData[i].TexCoord.x = batch->vertex[i].u;
			VertexData[i].TexCoord.y = batch->vertex[i].v;

			VertexData[i].Color.x = batch->vertex[i].r / 255.f;
			VertexData[i].Color.y = batch->vertex[i].g / 255.f;
			VertexData[i].Color.z = batch->vertex[i].b / 255.f;
			VertexData[i].Color.w = batch->vertex[i].a / 255.f;
		}

		//For Debugging
		UIVertex FullScreenQuad[6] = {
			Vector2(), Vector2(), Vector4(1, 1, 1, 1),
			Vector2(0, g_Renderer.Size().y), Vector2(0, 1), Vector4(1, 1, 1, 1),
			g_Renderer.Size(), Vector2(1, 1), Vector4(1, 1, 1, 1),
			g_Renderer.Size(), Vector2(1, 1), Vector4(1, 1, 1, 1),
			Vector2(g_Renderer.Size().x, 0), Vector2(1, 0), Vector4(1, 1, 1, 1),
			Vector2(), Vector2(), Vector4(1, 1, 1, 1)
		};

		g_Renderer.SetVertexBufferData(VertexHandle, VertexDetailsMode::Mixed, UIFormat, sizeof(UIFormat) / sizeof(UIFormat[0]), &VertexData[0], VertexData.size() * sizeof(VertexData[0]));

		//For Debugging
		//g_Renderer.SetVertexBufferData(VertexHandle, VertexDetailsMode::Mixed, UIFormat, sizeof(UIFormat) / sizeof(UIFormat[0]), &FullScreenQuad[0], sizeof(FullScreenQuad));

		UIBitmap *TheBitmap = (UIBitmap *)batch->bitmap;

		if (TheBitmap)
		{
			TheBitmap->ContainedTexture->Bind();
		}
		else
		{
			g_Renderer.BindTexture((TextureHandle)INVALID_FTGHANDLE);
		}

		g_Renderer.SetBlendingMode(BlendingMode::Alpha);

		g_Renderer.RenderVertices(VertexModes::Triangles, VertexHandle, 0, VertexData.size());

		//For Debugging
		//g_Renderer.RenderVertices(VertexModes::Triangles, VertexHandle, 0, 6);
	}

	void UIRenderer::SetClipRect(const TBRect &rect)
	{
		g_Renderer.SetClipRect(Rect((f32)rect.x, (f32)rect.x + (f32)rect.w, (f32)rect.y, (f32)rect.y + (f32)rect.h));
	}

	MODIFIER_KEYS MapTBModifiers(const Input::KeyInfo &Key)
	{
		MODIFIER_KEYS Out = TB_MODIFIER_NONE;

		if (Key.Alt)
			Out |= TB_ALT;

		if (Key.Control)
			Out |= TB_CTRL;

		if (Key.Shift)
			Out |= TB_SHIFT;

		return Out;
	}

	SPECIAL_KEY MapTBSpecialKey(const Input::KeyInfo &Key)
	{
		switch (Key.Name)
		{
		case InputKey::F1:

			return TB_KEY_F1;

		case InputKey::F2:

			return TB_KEY_F2;

		case InputKey::F3:

			return TB_KEY_F3;

		case InputKey::F4:

			return TB_KEY_F4;

		case InputKey::F5:

			return TB_KEY_F5;

		case InputKey::F6:

			return TB_KEY_F6;

		case InputKey::F7:

			return TB_KEY_F7;

		case InputKey::F8:

			return TB_KEY_F8;

		case InputKey::F9:

			return TB_KEY_F9;

		case InputKey::F10:

			return TB_KEY_F10;

		case InputKey::F11:

			return TB_KEY_F11;

		case InputKey::F12:

			return TB_KEY_F12;

		case InputKey::Left:

			return TB_KEY_LEFT;

		case InputKey::Right:

			return TB_KEY_RIGHT;

		case InputKey::Up:

			return TB_KEY_UP;

		case InputKey::Down:

			return TB_KEY_DOWN;

		case InputKey::PageUp:

			return TB_KEY_PAGE_UP;

		case InputKey::PageDown:

			return TB_KEY_PAGE_DOWN;

		case InputKey::Home:

			return TB_KEY_HOME;

		case InputKey::End:

			return TB_KEY_END;

		case InputKey::Insert:

			return TB_KEY_INSERT;

		case InputKey::Tab:

			return TB_KEY_TAB;

		case InputKey::Delete:

			return TB_KEY_DELETE;

		case InputKey::BackSpace:

			return TB_KEY_BACKSPACE;

		case InputKey::Return:

			return TB_KEY_ENTER;

		case InputKey::Escape:

			return TB_KEY_ESC;
		}

		return TB_KEY_UNDEFINED;
	}

	bool IsTBSpecialKey(const Input::KeyInfo &Key)
	{
		return TB_KEY_UNDEFINED != MapTBSpecialKey(Key);
	}

	uint32 MapTBKey(const Input::KeyInfo &Key)
	{
		if (IsTBSpecialKey(Key))
			return 0;

		//TODO

		return 0;
	}

	UIInputProcessor::UIInputProcessor() : CurrentModifiers(TB_MODIFIER_NONE)
	{
	}

	bool UIInputProcessor::OnKey(const Input::KeyInfo &Key)
	{
		DisposablePointer<TBWidget> UIRoot = g_Renderer.UIRoot;

		bool Consume = false;

		CurrentModifiers = MapTBModifiers(Key);

		//TODO
		if (Key.JustPressed)
		{
			Consume = UIRoot->InvokeKey(MapTBKey(Key), MapTBSpecialKey(Key), CurrentModifiers, true);
		}
		else if (Key.Pressed)
		{
			//Consume = UIRoot->InvokeKey(MapTBKey(Key), MapTBSpecialKey(Key), CurrentModifiers, true);
		}
		else if (Key.JustReleased)
		{
			Consume = UIRoot->InvokeKey(MapTBKey(Key), MapTBSpecialKey(Key), CurrentModifiers, false);
		}

		if (Consume)
		{
			g_Input.ConsumeInput();
		}

		return Consume;
	}

	bool UIInputProcessor::OnTouch(const Input::TouchInfo &Touch)
	{
		DisposablePointer<TBWidget> UIRoot = g_Renderer.UIRoot;
		ClickCounter &TheClickCounter = MouseClicks[Touch.Index];

		bool Consume = false;

		//TODO
		if (Touch.JustPressed)
		{
			if (GameClockDiffNoPause(TheClickCounter.ClickTimer) > 600)
			{
				TheClickCounter.Count = 0;
			}

			TheClickCounter.Count++;
			TheClickCounter.ClickTimer = GameClockTimeNoPause();

			//For debugging
			//g_Log.LogInfo("UIInputProcessor", "Found Touch Down from UI, ClickCounter for touch TOUCH_%s count is %d", Touch.NameAsString().c_str(), TheClickCounter.Count);

			Consume = UIRoot->InvokePointerDown((int32)Touch.Position.x, (int32)Touch.Position.y, TheClickCounter.Count, CurrentModifiers, true);
		}
		else if (Touch.Pressed)
		{
			//Ignored
		}

		if (Touch.Dragged)
		{
			UIRoot->InvokePointerMove((int32)Touch.Position.x, (int32)Touch.Position.y, CurrentModifiers, true);
		}

		if (Touch.JustReleased)
		{
			Consume = UIRoot->InvokePointerUp((int32)Touch.Position.x, (int32)Touch.Position.y, CurrentModifiers, true);
		}

		if (Consume)
		{
			g_Input.ConsumeInput();
		}

		return g_Input.InputConsumed();
	}

	bool UIInputProcessor::OnMouseButton(const Input::MouseButtonInfo &Button)
	{
		DisposablePointer<TBWidget> UIRoot = g_Renderer.UIRoot;
		ClickCounter &TheClickCounter = MouseClicks[Button.Name];

		bool Consume = false;

		if (Button.JustPressed)
		{
			if (GameClockDiffNoPause(TheClickCounter.ClickTimer) > 600)
			{
				TheClickCounter.Count = 0;
			}

			TheClickCounter.Count++;
			TheClickCounter.ClickTimer = GameClockTimeNoPause();

			//For debugging
			//g_Log.LogInfo("UIInputProcessor", "Found Mouse Down from UI, ClickCounter for button MOUSEBUTTON_%s count is %d", Button.NameAsString().c_str(), TheClickCounter.Count);

			Consume = UIRoot->InvokePointerDown((int32)g_Input.MousePosition.x, (int32)g_Input.MousePosition.y, TheClickCounter.Count, CurrentModifiers, false);
		}
		else if (Button.Pressed)
		{
		}
		else if (Button.JustReleased)
		{
			Consume = UIRoot->InvokePointerUp((int32)g_Input.MousePosition.x, (int32)g_Input.MousePosition.y, CurrentModifiers, false);
		}

		if (Consume)
		{
			g_Input.ConsumeInput();
		}

		return g_Input.InputConsumed();
	}

	bool UIInputProcessor::OnJoystickButton(const Input::JoystickButtonInfo &Button)
	{
		//TODO
		if (Button.JustPressed)
		{
		}
		else if (Button.Pressed)
		{
		}
		else if (Button.JustReleased)
		{
		}

		return g_Input.InputConsumed();
	}

	bool UIInputProcessor::OnJoystickAxis(const Input::JoystickAxisInfo &Axis)
	{
		//TODO

		return g_Input.InputConsumed();
	}

	void UIInputProcessor::OnJoystickConnected(uint8 Index)
	{
	}

	void UIInputProcessor::OnJoystickDisconnected(uint8 Index)
	{
	}

	void UIInputProcessor::OnMouseMove(const Vector2 &Position)
	{
		DisposablePointer<TBWidget> UIRoot = g_Renderer.UIRoot;

		UIRoot->InvokePointerMove((int32)g_Input.MousePosition.x, (int32)g_Input.MousePosition.y, CurrentModifiers, false);

		if (g_Input.MouseWheel != 0)
		{
			UIRoot->InvokeWheel((int32)g_Input.MousePosition.x, (int32)g_Input.MousePosition.y, 0, -(int32)g_Input.MouseWheel, CurrentModifiers);
		}
	}

	void UIInputProcessor::OnCharacterEntered(uint32 Character)
	{
		DisposablePointer<TBWidget> UIRoot = g_Renderer.UIRoot;

		SPECIAL_KEY Special = TB_KEY_UNDEFINED;

		switch (Character)
		{
		case 8: //Backspace
			Special = TB_KEY_BACKSPACE;
			Character = 0;

			break;
		case 13: //Return
			Special = TB_KEY_ENTER;
			Character = 0;

			break;
		}

		//Special Keys trigger their effect twice for some reason
		if (Character != 0)
			UIRoot->InvokeKey(Character, Special, TB_MODIFIER_NONE, true);

		UIRoot->InvokeKey(Character, Special, TB_MODIFIER_NONE, false);
	}

	void UIInputProcessor::OnAction(const Input::Action &TheAction)
	{
		//TODO
	}

	void UIInputProcessor::OnGainFocus() {}
	void UIInputProcessor::OnLoseFocus() {}
}
#endif
