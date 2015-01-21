#include "FlamingCore.hpp"

namespace FlamingTorch
{
#	if USE_GRAPHICS && USE_SDL_RENDERER
#	include "SDLRenderer.hpp"
#	define TAG "SDLRenderer"
#	define GLCHECK() { int32 error = glGetError(); if(error != GL_NO_ERROR) { FLASSERT(0, "GL Error %08x!", error); } }
#	define ONE_MB_FLOAT 1048576.f
#	define BUFFER_OFFSET(x) ((char *)NULL + x)
#	define SDL_RENDERER_VERSION_MAJOR 0
#	define SDL_RENDERER_VERSION_MINOR 1

	/*!
	*	We probably have some leaks here since we don't cleanup after ourselves!
	*/

	bool SDLRendererImplementation::FirstRenderer = true;
	uint64 SDLRendererImplementation::TextureCounter = 0;
	uint64 SDLRendererImplementation::VertexBufferCounter = 0;
	uint64 SDLRendererImplementation::FrameBufferCounter = 0;
	bool SDLRendererImplementation::ExtensionsAvailable = false;
	bool SDLRendererImplementation::SupportsVBOs = false;
	bool SDLRendererImplementation::SupportsFBOs = false;
	GLint SDLRendererImplementation::MaximumTextureSize = 0;

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

	uint32 VertexBufferVertexMode[VertexModes::Count] = {
		GL_TRIANGLES,
		GL_LINES,
		GL_POINTS
	};

	uint32 TranslateKeySym(SDL_Keysym sym)
	{
		//Should only try to translate hardware keys using the current layout
		//Doesn't make sense to make it work with custom symbols that may vary depending on layout
		switch (sym.sym)
		{
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
			return InputKey::Return;

		case SDLK_ESCAPE:

			return InputKey::Escape;

		case SDLK_BACKSPACE:

			return InputKey::BackSpace;

		case SDLK_TAB:

			return InputKey::Tab;

		case SDLK_SPACE:
		case SDLK_KP_SPACE:

			return InputKey::Space;

		case SDLK_PLUS:
		case SDLK_KP_PLUS:

			return InputKey::Add;

		case SDLK_MINUS:

			return InputKey::Subtract;

		case SDLK_ASTERISK:
		case SDLK_KP_MULTIPLY:

			return InputKey::Multiply;

		case SDLK_SLASH:
		case SDLK_KP_DIVIDE:

			return InputKey::Slash;

		case SDLK_0:
		case SDLK_1:
		case SDLK_2:
		case SDLK_3:
		case SDLK_4:
		case SDLK_5:
		case SDLK_6:
		case SDLK_7:
		case SDLK_8:
		case SDLK_9:

			return InputKey::Num0 + sym.sym - SDLK_0;

		case SDLK_SEMICOLON:
			
			return InputKey::SemiColon;

		case SDLK_PERIOD:
		case SDLK_KP_PERIOD:

			return InputKey::Period;

		case SDLK_LEFTBRACKET:

			return InputKey::LBracket;

		case SDLK_RIGHTBRACKET:

			return InputKey::RBracket;

		case SDLK_BACKSLASH:

			return InputKey::BackSlash;

		case SDLK_a:
		case SDLK_b:
		case SDLK_c:
		case SDLK_d:
		case SDLK_e:
		case SDLK_f:
		case SDLK_g:
		case SDLK_h:
		case SDLK_i:
		case SDLK_j:
		case SDLK_k:
		case SDLK_l:
		case SDLK_m:
		case SDLK_n:
		case SDLK_o:
		case SDLK_p:
		case SDLK_q:
		case SDLK_r:
		case SDLK_s:
		case SDLK_t:
		case SDLK_u:
		case SDLK_v:
		case SDLK_w:
		case SDLK_x:
		case SDLK_y:
		case SDLK_z:

			return InputKey::A + (SDLK_z - sym.sym);

		case SDLK_F1:

			return InputKey::F1;

		case SDLK_F2:

			return InputKey::F2;

		case SDLK_F3:

			return InputKey::F3;

		case SDLK_F4:

			return InputKey::F4;

		case SDLK_F5:

			return InputKey::F5;

		case SDLK_F6:

			return InputKey::F6;

		case SDLK_F7:

			return InputKey::F7;

		case SDLK_F8:

			return InputKey::F8;

		case SDLK_F9:

			return InputKey::F9;

		case SDLK_F10:

			return InputKey::F10;

		case SDLK_F11:

			return InputKey::F11;

		case SDLK_F12:

			return InputKey::F12;

		case SDLK_PAUSE:

			return InputKey::Pause;

		case SDLK_INSERT:

			return InputKey::Insert;

		case SDLK_HOME:

			return InputKey::Home;

		case SDLK_PAGEUP:

			return InputKey::PageUp;

		case SDLK_PAGEDOWN:

			return InputKey::PageDown;

		case SDLK_DELETE:

			return InputKey::Delete;

		case SDLK_END:

			return InputKey::End;

		case SDLK_LEFT:

			return InputKey::Left;

		case SDLK_RIGHT:

			return InputKey::Right;

		case SDLK_UP:

			return InputKey::Up;

		case SDLK_DOWN:

			return InputKey::Down;

		case SDLK_KP_1:

			return InputKey::Num1;

		case SDLK_KP_2:

			return InputKey::Num2;

		case SDLK_KP_3:

			return InputKey::Num3;

		case SDLK_KP_4:

			return InputKey::Num4;

		case SDLK_KP_5:

			return InputKey::Num5;

		case SDLK_KP_6:

			return InputKey::Num6;

		case SDLK_KP_7:

			return InputKey::Num7;

		case SDLK_KP_8:

			return InputKey::Num8;

		case SDLK_KP_9:

			return InputKey::Num9;

		case SDLK_KP_0:

			return InputKey::Num0;

		case SDLK_LCTRL:

			return InputKey::LControl;

		case SDLK_RCTRL:

			return InputKey::RControl;

		case SDLK_LSHIFT:

			return InputKey::LShift;

		case SDLK_RSHIFT:

			return InputKey::RShift;

		case SDLK_LALT:

			return InputKey::LAlt;

		case SDLK_RALT:

			return InputKey::RAlt;

		}

		return InputKey::Count;
	}

	void DumpRendererStats()
	{
		Log::Instance.LogInfo(TAG, "GL Info");
		Log::Instance.LogInfo(TAG, "   Renderer: %s", glGetString(GL_RENDERER));
		Log::Instance.LogInfo(TAG, "   Vendor: %s", glGetString(GL_VENDOR));
		Log::Instance.LogInfo(TAG, "   Version: %s", glGetString(GL_VERSION));

		int32 t, t2, t3;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &SDLRendererImplementation::MaximumTextureSize);

		GLCHECK();

		Log::Instance.LogInfo(TAG, "   Max Texture Size: %d", SDLRendererImplementation::MaximumTextureSize);

		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &t);
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &t2);
		glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &t3);

		GLCHECK();

		Log::Instance.LogInfo(TAG, "   Max Texture Units: %d (%d vs, %d ps)", t, t3, t2);

		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &t);

		GLCHECK();

		Log::Instance.LogInfo(TAG, "   Max Vertex Attribs: %d", t);
	}

	SDLRendererImplementation::SDLRendererImplementation() : LastBoundTexture(0), LastBoundVBO(0), 
		UniqueCacheStringID(0), SavedTextDrawcalls(0), BorderlessWindowMode(false)
	{
		FrameStatsValue.RendererName = "SDL";
		FrameStatsValue.RendererVersion = CoreUtils::MakeVersionString(SDL_RENDERER_VERSION_MAJOR, SDL_RENDERER_VERSION_MINOR);
		FrameStatsValue.RendererCustomMessage = "";
	}

	SDLRendererImplementation::~SDLRendererImplementation()
	{
		while(Textures.size())
		{
			DestroyTexture(Textures.begin()->first);
		}

		while (VertexBuffers.size())
		{
			DestroyVertexBuffer(VertexBuffers.begin()->first);
		}

		while (FrameBuffers.size())
		{
			DestroyFrameBuffer(FrameBuffers.begin()->first);
		}
	}

	bool SDLRendererImplementation::Create(void *WindowHandle, RendererCapabilities ExpectedCaps)
	{
		if (FirstRenderer)
		{
			if (SDL_Init(SDL_INIT_VIDEO) < 0)
			{
				Log::Instance.LogErr(TAG, "Failed to initialize video!");

				return false;
			}

			SDL_StartTextInput();
		}

		//Create the window
		Window = SDL_CreateWindowFrom(WindowHandle);

		if (!Window)
		{
			Log::Instance.LogErr(TAG, "Unable to create the rendering window.");

			return false;
		}

		GLContext = SDL_GL_CreateContext(Window);

		if (GLContext == NULL)
		{
			Log::Instance.LogErr(TAG, "Unable to create the rendering window.");

			SDL_DestroyWindow(Window);

			Window = NULL;

			return false;
		}

		GLCHECK();

		//If the first renderer, init glew
		if(FirstRenderer)
		{
			FirstRenderer = false;

#if FLPLATFORM_ANDROID
			ExtensionsAvailable = false;
#else

			if (PlatformInfo::PlatformType == PlatformType::Mobile)
			{
				ExtensionsAvailable = false;
			}
			else
			{
				GLenum err = glewInit();

				if (GLEW_OK != err)
				{
					Log::Instance.LogInfo(TAG, "GLEW failed to start, so no fancy OpenGL extensions will be available. Error Message: %s",
						glewGetErrorString(err));

					ExtensionsAvailable = false;
				}
				else
				{
					ExtensionsAvailable = true;
					SupportsFBOs = !!glewIsSupported("GL_ARB_framebuffer_object");
					//Disabled for now
					//SupportsVBOs = !!glewIsSupported("GL_ARB_vertex_buffer_object");
				}
			}
#endif

			GLCHECK();

			DumpRendererStats();
		}

		glDisable(GL_DEPTH_TEST);

		return true;
	}

	bool SDLRendererImplementation::Create(const std::string &Title, uint32 Width, uint32 Height, uint32 Style, RendererCapabilities ExpectedCaps)
	{
		OriginalRequestedSize = Vector2((f32)Width, (f32)Height);

		uint32 ActualStyle = SDL_WINDOW_OPENGL;

		switch(Style)
		{
		case RendererWindowStyle::FullScreen:
			ActualStyle |= SDL_WINDOW_FULLSCREEN;

			break;

		case RendererWindowStyle::Popup:
			ActualStyle |= SDL_WINDOW_FULLSCREEN_DESKTOP;
			BorderlessWindowMode = true;

			break;
		}

		if (FirstRenderer)
		{
			if(SDL_Init(SDL_INIT_VIDEO) < 0)
			{
				Log::Instance.LogErr(TAG, "Failed to initialize video!");

				return false;
			}

			SDL_StartTextInput();
		}

		uint32 ActualWidth = Width, ActualHeight = Height;

#if FLPLATFORM_ANDROID
		ActualWidth = DesktopDisplayMode().Width;
		ActualHeight = DesktopDisplayMode().Height;
#else
		Rect WorkArea = CoreUtils::GetDesktopWorkArea();

		if(WorkArea.Right != 0)
		{
			if(ActualWidth > WorkArea.Right - WorkArea.Left)
			{
				ActualWidth = (uint32)(WorkArea.Right - WorkArea.Left);
			}

			if(ActualHeight > WorkArea.Bottom - WorkArea.Top)
			{
				ActualHeight = (uint32)(WorkArea.Bottom - WorkArea.Top);
			}
		}
#endif

		//Create the window
		Window = SDL_CreateWindow(Title.c_str(), WorkArea.Left == 0 ? SDL_WINDOWPOS_CENTERED : WorkArea.Left, WorkArea.Top == 0 ? SDL_WINDOWPOS_CENTERED : WorkArea.Top, ActualWidth, ActualHeight, ActualStyle);

		if (!Window)
		{
			Log::Instance.LogErr(TAG, "Unable to create the rendering window.");

			return false;
		}

		GLContext = SDL_GL_CreateContext(Window);

		if (GLContext == NULL)
		{
			Log::Instance.LogErr(TAG, "Unable to create the rendering window.");

			SDL_DestroyWindow(Window);

			Window = NULL;

			return false;
		}

		GLCHECK();

		//If the first renderer, init glew
		if(FirstRenderer)
		{
			FirstRenderer = false;

#if FLPLATFORM_ANDROID
			ExtensionsAvailable = false;
#else

			if (PlatformInfo::PlatformType == PlatformType::Mobile)
			{
				ExtensionsAvailable = false;
			}
			else
			{
				GLenum err = glewInit();

				if (GLEW_OK != err)
				{
					Log::Instance.LogInfo(TAG, "GLEW failed to start, so no fancy OpenGL extensions will be available. Error Message: %s",
						glewGetErrorString(err));

					ExtensionsAvailable = false;
				}
				else
				{
					ExtensionsAvailable = true;
					SupportsFBOs = !!glewIsSupported("GL_ARB_framebuffer_object");
					//Disabled for now
					//SupportsVBOs = !!glewIsSupported("GL_ARB_vertex_buffer_object");
				}
			}
#endif

			GLCHECK();

			DumpRendererStats();
		}

		glDisable(GL_DEPTH_TEST);

		return true;
	}

	FrameBufferHandle SDLRendererImplementation::CreateFrameBuffer(const FrameBufferCreationInfo &Info)
	{
		if (!ExtensionsAvailable || !SupportsFBOs || Info.Size.x <= 0 || Info.Size.y <= 0)
			return INVALID_FTGHANDLE;

		for (uint32 i = 0; i < Info.ColorBuffers.size(); i++)
		{
			if (Info.ColorBuffers[i].Get() == NULL || Info.ColorBuffers[i]->Size() != Info.Size || !IsTextureHandleValid(Info.ColorBuffers[i]->Handle()))
				return INVALID_FTGHANDLE;
		}

		DisposablePointer<FrameBufferInfo> FBInfo(new FrameBufferInfo());

		GLCHECK();

		glGenFramebuffers(1, &FBInfo->GLID);

		GLCHECK();

		glBindFramebuffer(GL_FRAMEBUFFER, FBInfo->GLID);

		GLCHECK();

		std::vector<GLuint> DrawBuffers;
		TextureHandle PreviousHandle = LastBoundTexture;

		BindTexture((TextureHandle)0);

		for (uint32 i = 0; i < Info.ColorBuffers.size(); i++)
		{
			BindTexture(Info.ColorBuffers[i]->Handle());

			glGenerateMipmap(GL_TEXTURE_2D);

			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, Textures[Info.ColorBuffers[i]->Handle()].GLID, 0);

			GLCHECK();

			DrawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
		}

		BindTexture(PreviousHandle);

		glGenRenderbuffers(1, &FBInfo->RenderBufferID);

		GLCHECK();

		glBindRenderbuffer(GL_RENDERBUFFER, FBInfo->RenderBufferID);

		GLCHECK();

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, (GLsizei)Info.Size.x, (GLsizei)Info.Size.y);

		GLCHECK();

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, FBInfo->RenderBufferID);

		GLCHECK();

		glDrawBuffers(DrawBuffers.size(), &DrawBuffers[0]);

		GLCHECK();

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glDeleteRenderbuffers(1, &FBInfo->RenderBufferID);
			glDeleteFramebuffers(1, &FBInfo->GLID);

			return INVALID_FTGHANDLE;
		}

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		FrameBuffers[++FrameBufferCounter] = FBInfo;

		return FrameBufferCounter;
	}

	bool SDLRendererImplementation::IsFrameBufferValid(FrameBufferHandle Handle)
	{
		FrameBufferMap::iterator it = FrameBuffers.find(Handle);

		return it != FrameBuffers.end();
	}

	void SDLRendererImplementation::BindFrameBuffer(FrameBufferHandle Handle)
	{
		if (!IsFrameBufferValid(Handle))
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffers[Handle]->GLID);
	}

	void SDLRendererImplementation::DestroyFrameBuffer(FrameBufferHandle Handle)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (!IsFrameBufferValid(Handle))
			return;

		FrameBufferMap::iterator it = FrameBuffers.find(Handle);

		glDeleteRenderbuffers(1, &it->second->RenderBufferID);
		glDeleteFramebuffers(1, &it->second->GLID);

		FrameBuffers.erase(it);
	}

	const RendererCapabilities &SDLRendererImplementation::Capabilities() const
	{
		return RenderCaps;
	}

	RendererDisplayMode SDLRendererImplementation::DesktopDisplayMode()
	{
		SDL_DisplayMode DisplayMode;

		if (SDL_GetDesktopDisplayMode(0, &DisplayMode) < 0)
			return RendererDisplayMode();

		RendererDisplayMode Out;

		Out.Width = DisplayMode.w;
		Out.Height = DisplayMode.h;
		Out.Bpp = SDL_BITSPERPIXEL(DisplayMode.format);

		return Out;
	}

	std::vector<RendererDisplayMode> SDLRendererImplementation::DisplayModes()
	{
		std::vector<RendererDisplayMode> Out;

		for (uint32 i = 0; i < SDL_GetNumDisplayModes(0); i++)
		{
			SDL_DisplayMode DisplayMode;

			if (SDL_GetDisplayMode(0, i, &DisplayMode) < 0)
				continue;

			if (SDL_BITSPERPIXEL(DisplayMode.format) != 32)
				continue;

			RendererDisplayMode OutMode;
			OutMode.Width = DisplayMode.w;
			OutMode.Height = DisplayMode.h;
			OutMode.Bpp = SDL_BITSPERPIXEL(DisplayMode.format);

			Out.push_back(OutMode);
		}

		return Out;
	}

	Vector2 SDLRendererImplementation::Size() const
	{
		int32 Width, Height;

		SDL_GetWindowSize(Window, &Width, &Height);

		return Vector2((f32)Width, (f32)Height);
	}

	VertexBufferHandle SDLRendererImplementation::CreateVertexBuffer()
	{
		FrameStatsValue.StateChanges++;

		VertexBufferCounter++;

		VertexBufferInfo &Info = VertexBuffers[VertexBufferCounter];
		Info.VertexCount = 0;
		Info.VertexSize = 0;

		if(SupportsVBOs)
		{
			glGenBuffers(1, &Info.VBOID);
		}

		return VertexBufferCounter;
	}

	void SDLRendererImplementation::SetVertexBufferData(VertexBufferHandle Handle, uint8 DetailsMode, VertexElementDescriptor *Elements, uint32 ElementCount, const void *Data, uint32 DataByteSize)
	{
		FrameStatsValue.StateChanges++;

		VertexBufferMap::iterator it = VertexBuffers.find(Handle);

		if(it == VertexBuffers.end())
			return;

		uint32 VertexSize = 0;
		int32 PositionOffset = -1, TexCoordOffset = -1, NormalOffset = -1, ColorOffset = -1;
		uint32 PositionDataType = 0, TexCoordDataType = 0, NormalDataType = 0, ColorDataType = 0;
		uint32 PositionDataSize = 0, TexCoordDataSize = 0, NormalDataSize = 0, ColorDataSize = 0;

		for(uint32 i = 0, LastOffset = 0, LastElementSize = 0; i < ElementCount; i++)
		{
			if(Elements[i].Offset != LastOffset + LastElementSize)
			{
				Log::Instance.LogErr(TAG, "Expected a list of connected Offsets; Offset '%d' is not valid: Should be '%d'", Elements[i].Offset, LastOffset + LastElementSize);

				return;
			}

			if(Elements[i].DataType >= VertexElementDataType::Count)
			{
				Log::Instance.LogErr(TAG, "Unknown data type '%d'", Elements[i].DataType);

				return;
			}

			switch(Elements[i].Type)
			{
			case VertexElementType::Position:
				if(PositionOffset != -1)
				{
					Log::Instance.LogErr(TAG, "Found duplicate element for POSITION");

					return;
				}

				PositionOffset = Elements[i].Offset;
				PositionDataType = Elements[i].DataType;
				PositionDataSize = VertexBufferDataElementSizes[Elements[i].DataType];

				break;
			case VertexElementType::TexCoord:
				if(TexCoordOffset != -1)
				{
					Log::Instance.LogErr(TAG, "Found duplicate element for TEXCOORD");

					return;
				}

				TexCoordOffset = Elements[i].Offset;
				TexCoordDataType = Elements[i].DataType;
				TexCoordDataSize = VertexBufferDataElementSizes[Elements[i].DataType];

				break;

			case VertexElementType::Normal:
				if(NormalOffset != -1)
				{
					Log::Instance.LogErr(TAG, "Found duplicate element for NORMAL");

					return;
				}

				NormalOffset = Elements[i].Offset;
				NormalDataType = Elements[i].DataType;
				NormalDataSize = VertexBufferDataElementSizes[Elements[i].DataType];

				break;

			case VertexElementType::Color:
				if(ColorOffset != -1)
				{
					Log::Instance.LogErr(TAG, "Found duplicate element for COLOR");

					return;
				}

				ColorOffset = Elements[i].Offset;
				ColorDataType = Elements[i].DataType;
				ColorDataSize = VertexBufferDataElementSizes[Elements[i].DataType];

				break;

			default:
				Log::Instance.LogErr(TAG, "Unknown element type '%d'", Elements[i].Type);

				return;
			}

			LastOffset = Elements[i].Offset;
			LastElementSize = VertexBufferDataElementSizes[Elements[i].DataType];
		}

		if(PositionOffset == -1)
		{
			Log::Instance.LogErr(TAG, "Expected a Position in this buffer");

			return;
		}

		VertexSize = (uint32)MathUtils::Max((f32)VertexSize, (f32)PositionOffset + PositionDataSize);
		VertexSize = (uint32)MathUtils::Max((f32)VertexSize, (f32)TexCoordOffset + TexCoordDataSize);
		VertexSize = (uint32)MathUtils::Max((f32)VertexSize, (f32)ColorOffset + ColorDataSize);
		VertexSize = (uint32)MathUtils::Max((f32)VertexSize, (f32)NormalOffset + NormalDataSize);

		if(VertexSize == 0)
		{
			Log::Instance.LogErr(TAG, "Vertex Size is 0");

			return;
		}

		if(DataByteSize % VertexSize != 0)
		{
			Log::Instance.LogErr(TAG, "Data Byte Size is incompatible with estimated Vertex Size '%d': Not a multiple of Vertex Size.", VertexSize);

			return;
		}

		uint32 VertexCount = DataByteSize / VertexSize;

		VertexBufferInfo &Info = it->second;

		bool NeedsFullRealloc = Info.VertexCount * Info.VertexSize < VertexCount * VertexSize;

		Info.VertexCount = VertexCount;
		Info.VertexSize = VertexSize;
		Info.PositionDataType = PositionDataType;
		Info.TexCoordDataType = TexCoordDataType;
		Info.NormalsDataType = NormalDataType;
		Info.ColorDataType = ColorDataType;

		Info.PositionData.resize(VertexCount * PositionDataSize);

		if(TexCoordDataSize)
		{
			Info.TexCoordData.resize(VertexCount * TexCoordDataSize);
		}
		else
		{
			Info.TexCoordData.resize(0);
		}

		if(ColorDataSize)
		{
			Info.ColorData.resize(VertexCount * ColorDataSize);
		}
		else
		{
			Info.ColorData.resize(0);
		}

		if(NormalDataSize)
		{
			Info.NormalsData.resize(VertexCount * NormalDataSize);
		}
		else
		{
			Info.NormalsData.resize(0);
		}

		switch(DetailsMode)
		{
		case VertexDetailsMode::Lists:
			memcpy(&Info.PositionData[0], &((uint8 *)Data)[PositionOffset], PositionDataSize * VertexCount);

			if(TexCoordDataSize)
			{
				memcpy(&Info.TexCoordData[0], &((uint8 *)Data)[TexCoordOffset], TexCoordDataSize * VertexCount);
			}

			if(ColorDataSize)
			{
				memcpy(&Info.ColorData[0], &((uint8 *)Data)[ColorOffset], ColorDataSize * VertexCount);
			}

			if(NormalDataSize)
			{
				memcpy(&Info.NormalsData[0], &((uint8 *)Data)[NormalOffset], NormalDataSize * VertexCount);
			}

			break;

		case VertexDetailsMode::Mixed:
			{
				uint32 PositionByteOffset = 0, TexCoordByteOffset = 0, NormalByteOffset = 0, ColorByteOffset = 0;

				for(uint32 i = 0; i < DataByteSize; i += VertexSize)
				{
					if(PositionDataSize > 0)
					{
						memcpy(&Info.PositionData[PositionByteOffset], &((uint8 *)Data)[i + PositionOffset], PositionDataSize);

						PositionByteOffset += PositionDataSize;
					}

					if(ColorDataSize > 0)
					{
						memcpy(&Info.ColorData[ColorByteOffset], &((uint8 *)Data)[i + ColorOffset], ColorDataSize);

						ColorByteOffset += ColorDataSize;
					}

					if(TexCoordDataSize > 0)
					{
						memcpy(&Info.TexCoordData[TexCoordByteOffset], &((uint8 *)Data)[i + TexCoordOffset], TexCoordDataSize);

						TexCoordByteOffset += TexCoordDataSize;
					}

					if(NormalDataSize > 0)
					{
						memcpy(&Info.NormalsData[NormalByteOffset], &((uint8 *)Data)[i + NormalOffset], NormalDataSize);

						NormalByteOffset += NormalDataSize;
					}
				}
			}

			break;
		}

		if(SupportsVBOs)
		{
			Info.CombinedData.resize(Info.PositionData.size() + Info.TexCoordData.size() + Info.ColorData.size() + Info.NormalsData.size());

			uint32 Offset = 0;

			if(PositionDataSize)
			{
				Info.PositionOffset = 0;

				Offset += PositionDataSize;
			}
			else
			{
				Info.PositionOffset = -1;
			}

			if(TexCoordDataSize)
			{
				Info.TexCoordOffset = Offset;

				Offset += TexCoordDataSize;
			}
			else
			{
				Info.TexCoordOffset = -1;
			}

			if(ColorDataSize)
			{
				Info.ColorOffset = Offset;

				Offset += ColorDataSize;
			}
			else
			{
				Info.ColorOffset = -1;
			}

			if(NormalDataSize)
			{
				Info.NormalsOffset = Offset;

				Offset += NormalDataSize;
			}
			else
			{
				Info.NormalsOffset = -1;
			}

			for(uint32 i = 0, index = 0; i < DataByteSize; i += VertexSize, index++)
			{
				if(Info.PositionOffset != -1)
				{
					memcpy(&Info.CombinedData[i + Info.PositionOffset], &Info.PositionData[index * PositionDataSize], PositionDataSize);
				}

				if(Info.ColorOffset != -1)
				{
					memcpy(&Info.CombinedData[i + Info.ColorOffset], &Info.ColorData[index * ColorDataSize], ColorDataSize);
				}

				if(Info.TexCoordOffset != -1)
				{
					memcpy(&Info.CombinedData[i + Info.TexCoordOffset], &Info.TexCoordData[index * TexCoordDataSize], TexCoordDataSize);
				}

				if(Info.NormalsOffset != -1)
				{
					memcpy(&Info.CombinedData[i + Info.NormalsOffset], &Info.NormalsData[index * NormalDataSize], NormalDataSize);
				}
			}

			GLCHECK();

			glBindBuffer(GL_ARRAY_BUFFER, Info.VBOID);

			GLCHECK();

			if(NeedsFullRealloc)
			{
				glBufferData(GL_ARRAY_BUFFER, DataByteSize, &Info.CombinedData[0], GL_DYNAMIC_DRAW);

				GLCHECK();
			}
			else
			{
				glBufferSubData(GL_ARRAY_BUFFER, 0, DataByteSize, &Info.CombinedData[0]);

				GLCHECK();
			}

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			GLCHECK();

			LastBoundVBO = 0;
		}
	}

	bool SDLRendererImplementation::IsVertexBufferHandleValid(VertexBufferHandle Handle)
	{
		return Handle != 0 && VertexBuffers.find(Handle) != VertexBuffers.end();
	}

	void SDLRendererImplementation::DestroyVertexBuffer(VertexBufferHandle Handle)
	{
		FrameStatsValue.StateChanges++;

		VertexBufferMap::iterator it = VertexBuffers.find(Handle);

		if(it == VertexBuffers.end())
			return;

		if(SupportsVBOs)
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDeleteBuffers(1, &it->second.VBOID);
			LastBoundVBO = 0;
		}

		VertexBuffers.erase(it);
	}

	void SDLRendererImplementation::RenderVertices(uint32 VertexMode, VertexBufferHandle Buffer, uint32 Start, uint32 End)
	{
		FrameStatsValue.StateChanges++;

		VertexBufferMap::iterator it = VertexBuffers.find(Buffer);

		if(it == VertexBuffers.end())
		{
			Log::Instance.LogErr(TAG, "Unable to render a buffer '%d': Invalid Buffer", Buffer);

			return;
		}

		if(VertexMode >= VertexModes::Count)
		{
			Log::Instance.LogErr(TAG, "Unable to render a buffer '%d': Invalid Vertex Mode '%d'", Buffer, VertexMode);

			return;
		}

		if(End <= Start)
		{
			Log::Instance.LogErr(TAG, "Unable to render a buffer '%d': Invalid Start/End Pair '%d, %d'", Buffer, Start, End);

			return;
		}

		FrameStatsValue.DrawCalls++;
		FrameStatsValue.VertexCount += End - Start;

		if(LastBoundTexture != 0)
		{
			EnableState(GL_TEXTURE_2D);
		}
		else
		{
			DisableState(GL_TEXTURE_2D);
		}

		EnableState(GL_VERTEX_ARRAY);

		if(SupportsVBOs)
		{
			if(glIsBuffer(it->second.VBOID))
			{
				if(it->second.VBOID != LastBoundVBO)
				{
					glBindBuffer(GL_ARRAY_BUFFER, it->second.VBOID);
					GLCHECK();
					LastBoundVBO = it->second.VBOID;
				}
			}
			else
			{
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				GLCHECK();
				LastBoundVBO = 0;
			}
		}

		if(SupportsVBOs && glIsBuffer(it->second.VBOID))
		{
			glVertexPointer(VertexBufferDataElementCount[it->second.PositionDataType], GL_FLOAT, it->second.VertexSize, BUFFER_OFFSET(it->second.PositionOffset));
		}
		else
		{
			glVertexPointer(VertexBufferDataElementCount[it->second.PositionDataType], GL_FLOAT, 0, &it->second.PositionData[0]);
		}

		if(SupportsVBOs && glIsBuffer(it->second.VBOID) && it->second.TexCoordOffset != -1)
		{
			EnableState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(VertexBufferDataElementCount[it->second.TexCoordDataType], GL_FLOAT, it->second.VertexSize, BUFFER_OFFSET(it->second.TexCoordOffset));
		}
		else if(it->second.TexCoordData.size())
		{
			EnableState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(VertexBufferDataElementCount[it->second.TexCoordDataType], GL_FLOAT, 0, &it->second.TexCoordData[0]);
		}
		else
		{
			DisableState(GL_TEXTURE_COORD_ARRAY);
		}

		if(SupportsVBOs && glIsBuffer(it->second.VBOID) && it->second.ColorOffset != -1)
		{
			EnableState(GL_COLOR_ARRAY);
			glColorPointer(VertexBufferDataElementCount[it->second.ColorDataType], GL_FLOAT, it->second.VertexSize, BUFFER_OFFSET(it->second.ColorOffset));
		}
		else if(it->second.ColorData.size())
		{
			EnableState(GL_COLOR_ARRAY);
			glColorPointer(VertexBufferDataElementCount[it->second.ColorDataType], GL_FLOAT, 0, &it->second.ColorData[0]);
		}
		else
		{
			DisableState(GL_COLOR_ARRAY);
			glColor4f(1, 1, 1, 1);
		}

		if(SupportsVBOs && glIsBuffer(it->second.VBOID) && it->second.NormalsOffset != -1)
		{
			EnableState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, it->second.VertexSize, BUFFER_OFFSET(it->second.NormalsOffset));
		}
		else if(it->second.NormalsData.size())
		{
			EnableState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, 0, &it->second.NormalsData[0]);
		}
		else
		{
			DisableState(GL_NORMAL_ARRAY);
		}

		glDrawArrays(VertexBufferVertexMode[VertexMode], Start, End - Start);

		GLCHECK();
	}

	void SDLRendererImplementation::StartClipping(const Rect &ClippingRect)
	{
		FrameStatsValue.StateChanges++;
		FrameStatsValue.ClippingChanges++;

		EnableState(GL_SCISSOR_TEST);

		GLint x = (GLint)ClippingRect.Left, y = (GLint)(Size().y - ClippingRect.Bottom);

		GLsizei Width = (GLsizei)(ClippingRect.Right - ClippingRect.Left), Height = (GLsizei)(ClippingRect.Bottom - ClippingRect.Top);

		glScissor(x, y, Width, Height);

		GLCHECK();
	}

	void SDLRendererImplementation::FinishClipping()
	{
		FrameStatsValue.StateChanges++;
		FrameStatsValue.ClippingChanges++;

		DisableState(GL_SCISSOR_TEST);

		GLCHECK();
	}

	void SDLRendererImplementation::Clear(uint32 Buffers)
	{
		FrameStatsValue.StateChanges++;

		uint32 Mask = 0;

		if(Buffers & RendererBuffer::Color)
			Mask |= GL_COLOR_BUFFER_BIT;

		if(Buffers & RendererBuffer::Depth)
			Mask |= GL_DEPTH_BUFFER_BIT;

		if(Buffers & RendererBuffer::Stencil)
			Mask |= GL_STENCIL_BUFFER_BIT;

		if(Mask != 0)
			glClear(Mask);

		GLCHECK();
	}

	void SDLRendererImplementation::Display()
	{
		FrameStatsValue.RendererCustomMessage = "";

		if(ExtensionsAvailable)
		{
			FrameStatsValue.RendererCustomMessage += "SupportsExtensions";
		}

		if(SupportsVBOs)
		{
			FrameStatsValue.RendererCustomMessage += " VBOEnabled";
		}

		if(RenderCaps.AntialiasLevel == 0)
		{
			FrameStatsValue.RendererCustomMessage += " No-AA";
		}
		else
		{
			FrameStatsValue.RendererCustomMessage += " AAx" + StringUtils::MakeIntString(RenderCaps.AntialiasLevel);
		}

		FrameStatsValue.RendererCustomMessage += " RF: D" + StringUtils::MakeIntString(RenderCaps.DepthBits) + "S" + StringUtils::MakeIntString(RenderCaps.StencilBits);

		PreviousFrameStatsValue = FrameStatsValue;
		FrameStatsValue.Clear();

		FrameStatsValue.TotalResources = VertexBuffers.size() + Textures.size();
		FrameStatsValue.TotalResourceUsage = 0;

		for(VertexBufferMap::iterator it = VertexBuffers.begin(); it != VertexBuffers.end(); it++)
		{
			FrameStatsValue.TotalResourceUsage += (it->second.PositionData.size() + it->second.TexCoordData.size() + it->second.NormalsData.size() + it->second.ColorData.size()) / ONE_MB_FLOAT;
		}

		for(TextureMap::iterator it = Textures.begin(); it != Textures.end(); it++)
		{
			//Multiplied by 8 instead of 4 due to double RAM usage
			FrameStatsValue.TotalResourceUsage += (it->second.Width * it->second.Height * 8) / ONE_MB_FLOAT;
		}

		SavedTextDrawcalls = 0;

		SDL_GL_SwapWindow(Window);
	}

	const RendererFrameStats &SDLRendererImplementation::FrameStats() const
	{
		return PreviousFrameStatsValue;
	}

	void SDLRendererImplementation::SetWorldMatrix(const Matrix4x4 &WorldMatrix)
	{
		SpriteCache::Instance.Flush(Target);

		FrameStatsValue.StateChanges++;
		FrameStatsValue.MatrixChanges++;

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&WorldMatrix.m[0][0]);

		GLCHECK();

		LastWorldMatrix = WorldMatrix;
	}

	void SDLRendererImplementation::SetProjectionMatrix(const Matrix4x4 &ProjectionMatrix)
	{
		SpriteCache::Instance.Flush(Target);

		FrameStatsValue.StateChanges++;
		FrameStatsValue.MatrixChanges++;

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(&ProjectionMatrix.m[0][0]);
		glMatrixMode(GL_MODELVIEW);

		GLCHECK();

		LastProjectionMatrix = ProjectionMatrix;
	}

	void SDLRendererImplementation::SetViewport(f32 x, f32 y, f32 Width, f32 Height)
	{
		FrameStatsValue.StateChanges++;

		glViewport((GLint)x, (GLint)y, (GLsizei)Width, (GLsizei)Height);

		GLCHECK();
	}

	TextureHandle SDLRendererImplementation::CreateTexture()
	{
		FrameStatsValue.StateChanges++;

		uint32 GLID = 0;

		glGenTextures(1, (GLuint *)&GLID);
		GLCHECK();

		TextureCounter++;

		Textures[TextureCounter].GLID = GLID;

		return TextureCounter;
	}

	bool SDLRendererImplementation::IsTextureHandleValid(TextureHandle Handle)
	{
		return Handle != 0 && Textures.find(Handle) != Textures.end();
	}

	void SDLRendererImplementation::DestroyTexture(TextureHandle Handle)
	{
		if(Handle == 0)
			return;

		TextureMap::iterator it = Textures.find(Handle);

		if(it == Textures.end())
			return;

		FrameStatsValue.StateChanges++;

		glDeleteTextures(1, (GLuint *)&it->second.GLID);

		GLCHECK();

		Textures.erase(it);
	}

	void SDLRendererImplementation::SetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 Width, uint32 Height)
	{
		TextureHandle Last = LastBoundTexture;

		if (!IsTextureHandleValid(Handle))
		{
			if (Last)
				BindTexture(Last);

			return;
		}

		FrameStatsValue.StateChanges++;

		BindTexture(Handle);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Pixels);

		GLCHECK();

		TextureInfo &Info = Textures[Handle];

		Info.Width = Width;
		Info.Height = Height;

		BindTexture(Last);
	}

	void SDLRendererImplementation::SetTextureWrapMode(TextureHandle Handle, uint32 WrapMode)
	{
		TextureHandle Last = LastBoundTexture;

		BindTexture(Handle);
		
		if(!IsTextureHandleValid(Handle))
		{
			if(Last)
				BindTexture(Last);

			return;
		}

		FrameStatsValue.StateChanges++;

		switch(WrapMode)
		{
		case TextureWrapMode::Repeat:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			GLCHECK();

			break;

		case TextureWrapMode::ClampToBorder:
#if !FLPLATFORM_ANDROID //"Hack" to make this the same as CLAMP_TO_EDGE due to GLES not supporting this
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

			GLCHECK();

			break;
#endif

		case TextureWrapMode::Clamp:
#if !FLPLATFORM_ANDROID //"Hack" to make this the same as CLAMP_TO_EDGE due to GLES not supporting this
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			GLCHECK();

			break;
#endif

		case TextureWrapMode::ClampToEdge:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			GLCHECK();

			break;
		}

		BindTexture(Last);
	}

	void SDLRendererImplementation::SetTextureFiltering(TextureHandle Handle, uint32 Filter)
	{
		TextureHandle Last = LastBoundTexture;

		BindTexture(Handle);

		if(!IsTextureHandleValid(Handle))
		{
			if(Last)
				BindTexture(Last);

			return;
		}

		FrameStatsValue.StateChanges++;

		if(Filter == TextureFiltering::Nearest || Filter == TextureFiltering::Linear)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter == TextureFiltering::Nearest ? GL_NEAREST : GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter == TextureFiltering::Nearest ? GL_NEAREST : GL_LINEAR);

			GLCHECK();
		}
		else if(Filter == TextureFiltering::Nearest_Mipmap || Filter == TextureFiltering::Linear_Mipmap)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter == TextureFiltering::Nearest_Mipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter == TextureFiltering::Nearest_Mipmap ? GL_NEAREST : GL_LINEAR);

			GLCHECK();

#if FLPLATFORM_ANDROID
			//Unsure how to check if this is available?
			//glGenerateMipmapOES(GL_TEXTURE_2D);
#else
			glGenerateMipmap(GL_TEXTURE_2D);
#endif

			GLCHECK();
		}

		BindTexture(Last);
	}

	bool SDLRendererImplementation::CaptureScreen(uint8 *Pixels, uint32 BufferByteCount)
	{
		if(BufferByteCount != (uint32)(Size().x * Size().y * 4))
			return false;

		FrameStatsValue.StateChanges++;

		GLCHECK();

		//RGB Buffer
		//Need to read as RGB since in RGBA it gets weird artifacts
		std::vector<uint8> Temp((uint32)(Size().x * Size().y * 3));

		glReadPixels(0, 0, (GLsizei)Size().x, (GLsizei)Size().y, GL_RGB, GL_UNSIGNED_BYTE, &Temp[0]);

		//Convert from RGB to RGBA
		for(uint32 i = 0, x = 0; i < Temp.size(); i += 3, x += 4)
		{
			Pixels[x] = Temp[i];
			Pixels[x + 1] = Temp[i + 1];
			Pixels[x + 2] = Temp[i + 2];
			Pixels[x + 3] = 255;
		}

		GLCHECK();

		return true;
	}

	bool SDLRendererImplementation::GetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 BufferByteCount)
	{
#if FLPLATFORM_ANDROID
		Log::Instance.LogErr(TAG, "Unable to GetTextureData on GLES");

		return false;
#endif

		TextureHandle Last = LastBoundTexture;

		BindTexture(Handle);

		GLCHECK();

		if(!IsTextureHandleValid(Handle))
		{
			if(Last)
				BindTexture(Last);

			return false;
		}

		TextureInfo &Info = Textures[Handle];

		if(BufferByteCount != Info.Width * Info.Height * 4)
		{
			if(Last)
				BindTexture(Last);

			return false;
		}

		FrameStatsValue.StateChanges++;

#if !FLPLATFORM_ANDROID
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, Pixels);
#endif

		GLCHECK();

		BindTexture(Last);

		GLCHECK();

		return true;
	}

	void SDLRendererImplementation::SetBlendingMode(uint32 BlendingMode)
	{
		FrameStatsValue.StateChanges++;

		switch(BlendingMode)
		{
		case BlendingMode::None:
			DisableState(GL_BLEND);

			break;

		case BlendingMode::Alpha:
			EnableState(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			break;

		case BlendingMode::Additive:
			EnableState(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);

			break;

		case BlendingMode::Subtractive:
			EnableState(GL_BLEND);

#if FLPLATFORM_ANDROID //GL_FUNC_SUBTRACT gives error, gotta figure out how to fix this.
					   //If glBlendEquation is available, then GL_FUNC_SUBTRACT should as well.
			FLASSERT(0, "Unable to use subtractive blending on Android!");
#else
			glBlendEquation(GL_FUNC_SUBTRACT);
#endif

			break;
		}

		GLCHECK();
	}

	void SDLRendererImplementation::BindTexture(TextureHandle Handle)
	{
		if(Handle == LastBoundTexture)
			return;

		FrameStatsValue.TextureChanges++;

		LastBoundTexture = Handle;

		TextureMap::iterator it = Textures.find(Handle);

		glBindTexture(GL_TEXTURE_2D, it != Textures.end() ? it->second.GLID : 0);

		GLCHECK();
	}

	bool SDLRendererImplementation::PollEvent(RendererEvent &Out)
	{
		if(BorderlessWindowMode)
		{
			//Disabled till can figure out the problem with the projection
			/*
			Rect WorkArea = CoreUtils::GetDesktopWorkArea();

			Vector2 ExpectedSize = OriginalRequestedSize;

			if(ExpectedSize.x > WorkArea.Right - WorkArea.Left)
			{
				ExpectedSize.x = WorkArea.Right - WorkArea.Left;
			}

			if(ExpectedSize.y > WorkArea.Bottom - WorkArea.Top)
			{
				ExpectedSize.y = WorkArea.Bottom - WorkArea.Top;
			}

			if(ExpectedSize != Size() || Window.getPosition().x < WorkArea.Left || Window.getPosition().y < WorkArea.Top) 
			{
				Window.setSize(sf::Vector2u((uint32)ExpectedSize.x, (uint32)ExpectedSize.y));
				Window.setPosition(sf::Vector2i((uint32)WorkArea.Left, (uint32)WorkArea.Top));

				sf::View TheView(sf::FloatRect(0, 0, ExpectedSize.x, ExpectedSize.y));
				Window.setView(TheView);

				Target->OnResized(Target, (uint32)ExpectedSize.x, (uint32)ExpectedSize.y);

				LastWindowSize = ExpectedSize;
			}
			*/
		}

		static SDL_Event Event;

		if(SDL_PollEvent(&Event))
		{
			switch(Event.type)
			{
			case SDL_JOYBUTTONDOWN:
				Out.Type = RendererEventType::JoystickButtonPressed;
				Out.JoystickIndex = Event.jdevice.which;
				Out.JoystickButtonIndex = Event.jbutton.button;

				return true;

			case SDL_JOYBUTTONUP:
				Out.Type = RendererEventType::JoystickButtonReleased;
				Out.JoystickIndex = Event.jdevice.which;
				Out.JoystickButtonIndex = Event.jbutton.button;

				return true;

			case SDL_JOYDEVICEADDED:
				Out.Type = RendererEventType::JoystickConnected;
				Out.JoystickIndex = Event.jdevice.which;

				return true;

			case SDL_JOYDEVICEREMOVED:
				Out.Type = RendererEventType::JoystickDisconnected;
				Out.JoystickIndex = Event.jdevice.which;

				return true;

			case SDL_JOYAXISMOTION:
				Out.Type = RendererEventType::JoystickAxisMoved;
				Out.JoystickIndex = Event.jdevice.which;
				Out.JoystickAxisIndex = Event.jaxis.axis;

				if(abs(Event.jaxis.value) > JOYSTICKDEADZONE)
				{
					Out.JoystickAxisPosition = Event.jaxis.value / 32767.f;
				}
				else
				{
					Out.JoystickAxisPosition = 0;
				}

				return true;

			case SDL_KEYDOWN:
				{
					uint32 Key = TranslateKeySym(Event.key.keysym);

					if (Key != InputKey::Count)
					{
						Out.Type = RendererEventType::KeyPressed;
						Out.KeyCode = Key;
					}
				}

				return true;

			case SDL_KEYUP:
				{
					uint32 Key = TranslateKeySym(Event.key.keysym);

					if (Key != InputKey::Count)
					{
						Out.Type = RendererEventType::KeyReleased;
						Out.KeyCode = Key;
					}
				}

				return true;

			case SDL_MOUSEMOTION:
				if (PlatformInfo::PlatformType == PlatformType::Mobile)
				{
					for (uint32 i = 0; i < InputMouseButton::Count; i++)
					{
						if (RendererManager::Instance.Input.MouseButtons[i].Pressed)
						{
							Out.Type = RendererEventType::TouchDrag;
							Out.TouchIndex = i;
							Out.TouchPosition = Vector2((f32)Event.motion.x, (f32)Event.motion.y);

							break;
						}
					}
				}
				else
				{
					Out.Type = RendererEventType::MouseMoved;
					Out.MousePosition = Vector2((f32)Event.motion.x, (f32)Event.motion.y);
				}

				return true;

			case SDL_MOUSEWHEEL:
				if (PlatformInfo::PlatformType == PlatformType::PC)
				{
					Out.Type = RendererEventType::MouseDeltaMoved;
					Out.MouseDelta = (f32)Event.wheel.y;
				}

				return true;

			case SDL_MOUSEBUTTONDOWN:
				if (PlatformInfo::PlatformType == PlatformType::Mobile)
				{
					Out.Type = RendererEventType::TouchDown;
					Out.TouchIndex = Event.button.which;
					Out.TouchPosition = Vector2((f32)Event.button.x, (f32)Event.button.y);
				}
				else
				{
					Out.Type = RendererEventType::MouseButtonPressed;
					Out.MouseButtonIndex = Event.button.which;
				}

				return true;

			case SDL_MOUSEBUTTONUP:
				if (PlatformInfo::PlatformType == PlatformType::Mobile)
				{
					Out.Type = RendererEventType::TouchUp;
					Out.TouchIndex = Event.button.which;
					Out.TouchPosition = Vector2((f32)Event.button.x, (f32)Event.button.y);
				}
				else
				{
					Out.Type = RendererEventType::MouseButtonPressed;
					Out.MouseButtonIndex = Event.button.which;
				}

				return true;

			case SDL_QUIT:
				Out.Type = RendererEventType::WindowClosed;

				return true;

			case SDL_TEXTEDITING:
				Out.Type = RendererEventType::CharacterEntered;
				Out.TypedCharacter = Event.text.text[0];

				return true;

			case SDL_WINDOWEVENT_RESIZED:
				Out.Type = RendererEventType::WindowResized;
				Out.WindowSize = Vector2((f32)Event.window.data1, (f32)Event.window.data1);

				Target->OnResized(Target, (uint32)Out.WindowSize.x, (uint32)Out.WindowSize.y);

				return true;

			case SDL_WINDOWEVENT_FOCUS_GAINED:
				Out.Type = RendererEventType::WindowGotFocus;

				return true;

			case SDL_WINDOWEVENT_FOCUS_LOST:
				Out.Type = RendererEventType::WindowLostFocus;

				return true;

			case SDL_FINGERDOWN:
				Out.Type = RendererEventType::TouchDown;
				Out.TouchIndex = Event.tfinger.fingerId;
				Out.TouchPosition = Vector2((f32)Event.tfinger.x, (f32)Event.tfinger.y);

				return true;

			case SDL_FINGERUP:
				Out.Type = RendererEventType::TouchUp;
				Out.TouchIndex = Event.tfinger.fingerId;
				Out.TouchPosition = Vector2((f32)Event.tfinger.x, (f32)Event.tfinger.y);

				return true;

			case SDL_FINGERMOTION:
				Out.Type = RendererEventType::TouchDrag;
				Out.TouchIndex = Event.tfinger.fingerId;
				Out.TouchPosition = Vector2((f32)Event.tfinger.x, (f32)Event.tfinger.y);

				return true;
			}
		}

		return false;
	}

	void SDLRendererImplementation::SetMousePosition(const Vector2 &Position)
	{
		//TODO
	}

	void *SDLRendererImplementation::WindowHandle() const
	{
		//TODO
		return NULL;
	}

	void SDLRendererImplementation::SetFrameRate(uint32 FPS)
	{
		//TODO
	}

	bool SDLRendererImplementation::IsStateEnabled(uint32 ID) const
	{
		GLStatesMap::const_iterator it = GLStates.find(ID);

		if(it == GLStates.end())
		{
			if(ID == GL_DITHER || ID == GL_MULTISAMPLE)
				return true;

			return false;
		}

		return it->second;
	}

	void SDLRendererImplementation::EnableState(uint32 ID)
	{
		if(IsStateEnabled(ID))
			return;

		FrameStatsValue.StateChanges++;

		switch(ID)
		{
		case GL_VERTEX_ARRAY:
		case GL_TEXTURE_COORD_ARRAY:
		case GL_NORMAL_ARRAY:
		case GL_COLOR_ARRAY:
#if !FLPLATFORM_ANDROID
		case GL_EDGE_FLAG_ARRAY:
		case GL_FOG_COORD_ARRAY:
		case GL_INDEX_ARRAY:
		case GL_SECONDARY_COLOR_ARRAY:
#endif
			glEnableClientState(ID);

			break;

		default:
			glEnable(ID);

			break;
		}

		GLCHECK();

		GLStates[ID] = true;
	}

	void SDLRendererImplementation::DisableState(uint32 ID)
	{
		if(!IsStateEnabled(ID))
			return;

		FrameStatsValue.StateChanges++;

		switch(ID)
		{
		case GL_VERTEX_ARRAY:
		case GL_TEXTURE_COORD_ARRAY:
		case GL_NORMAL_ARRAY:
		case GL_COLOR_ARRAY:
#if !FLPLATFORM_ANDROID
		case GL_EDGE_FLAG_ARRAY:
		case GL_FOG_COORD_ARRAY:
		case GL_INDEX_ARRAY:
		case GL_SECONDARY_COLOR_ARRAY:
#endif
			glDisableClientState(ID);

			break;
		default:
			glDisable(ID);

			break;
		}

		GLStates[ID] = false;
	}

	void SDLRendererImplementation::ReportSkippedDrawCall()
	{
		FrameStatsValue.SkippedDrawCalls++;
	}

#	endif
}
