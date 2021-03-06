#include "FlamingCore.hpp"
#if FLPLATFORM_ANDROID
#	include <SFML/OpenGL.hpp>
#	include <SFML/System/Android/Activity.hpp>
#endif

namespace FlamingTorch
{
#	if USE_GRAPHICS && USE_SFML_RENDERER
#	include "SFMLRenderer.hpp"
#	define TAG "SFMLRenderer"
#	define GLCHECK() { int32 error = glGetError(); if(error != GL_NO_ERROR) { FLASSERT(0, "GL Error %08x!", error); } }
#	define ONE_MB_FLOAT 1048576.f
#	define BUFFER_OFFSET(x) ((char *)NULL + x)
#	define SFML_RENDERER_VERSION_MAJOR 0
#	define SFML_RENDERER_VERSION_MINOR 1

	bool SFMLRendererImplementation::FirstRenderer = true;
	uint64 SFMLRendererImplementation::TextureCounter = 0;
	uint64 SFMLRendererImplementation::VertexBufferCounter = 0;
	uint64 SFMLRendererImplementation::FrameBufferCounter = 0;
	bool SFMLRendererImplementation::ExtensionsAvailable = false;
	bool SFMLRendererImplementation::SupportsVBOs = false;
	bool SFMLRendererImplementation::SupportsFBOs = false;
	GLint SFMLRendererImplementation::MaximumTextureSize = 0;

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

	void DumpRendererStats()
	{
		Log::Instance.LogInfo(TAG, "GL Info");
		Log::Instance.LogInfo(TAG, "   Renderer: %s", glGetString(GL_RENDERER));
		Log::Instance.LogInfo(TAG, "   Vendor: %s", glGetString(GL_VENDOR));
		Log::Instance.LogInfo(TAG, "   Version: %s", glGetString(GL_VERSION));

		int32 t, t2, t3;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &SFMLRendererImplementation::MaximumTextureSize);

		GLCHECK();

		Log::Instance.LogInfo(TAG, "   Max Texture Size: %d", SFMLRendererImplementation::MaximumTextureSize);

		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &t);
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &t2);
		glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &t3);

		GLCHECK();

		Log::Instance.LogInfo(TAG, "   Max Texture Units: %d (%d vs, %d ps)", t, t3, t2);

		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &t);

		GLCHECK();

		Log::Instance.LogInfo(TAG, "   Max Vertex Attribs: %d", t);
	}

	SFMLRendererImplementation::SFMLRendererImplementation() : LastBoundTexture(0), LastBoundVBO(0), 
		UniqueCacheStringID(0), SavedTextDrawcalls(0), BorderlessWindowMode(false)
	{
		FrameStatsValue.RendererName = "SFML";
		FrameStatsValue.RendererVersion = CoreUtils::MakeVersionString(SFML_RENDERER_VERSION_MAJOR, SFML_RENDERER_VERSION_MINOR);
		FrameStatsValue.RendererCustomMessage = "";
	}

	SFMLRendererImplementation::~SFMLRendererImplementation()
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

	bool SFMLRendererImplementation::Create(void *WindowHandle, RendererCapabilities ExpectedCaps)
	{
		sf::ContextSettings ContextSettings(ExpectedCaps.DepthBits, ExpectedCaps.StencilBits, ExpectedCaps.AntialiasLevel);

		//Init from an existing window
		Window.create((sf::WindowHandle)WindowHandle, ContextSettings);

		GLCHECK();

		//If failed to open, return an error
		if(!Window.isOpen())
		{
			Log::Instance.LogInfo(TAG, "Unable to create the rendering window.");

			return 0;
		}

		ContextSettings = Window.getSettings();

		RenderCaps.DepthBits = ContextSettings.depthBits;
		RenderCaps.StencilBits = ContextSettings.stencilBits;
		RenderCaps.AntialiasLevel = ContextSettings.antialiasingLevel;

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
		glClearColor(Renderer::DefaultClearColor.x, Renderer::DefaultClearColor.y, Renderer::DefaultClearColor.z, Renderer::DefaultClearColor.w);

		return true;
	}

	bool SFMLRendererImplementation::Create(const std::string &Title, uint32 Width, uint32 Height, uint32 Style, RendererCapabilities ExpectedCaps)
	{
		OriginalRequestedSize = Vector2((f32)Width, (f32)Height);

		uint32 ActualStyle = sf::Style::None;

		switch(Style)
		{
		case RendererWindowStyle::FullScreen:
			ActualStyle = sf::Style::Fullscreen;

			break;

		case RendererWindowStyle::Popup:
			ActualStyle = sf::Style::None;
			BorderlessWindowMode = true;

			break;

		default:
			ActualStyle = sf::Style::Close;

			break;
		}

		sf::ContextSettings ContextSettings(ExpectedCaps.DepthBits, ExpectedCaps.StencilBits, ExpectedCaps.AntialiasLevel);

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
		Window.create(sf::VideoMode(ActualWidth, ActualHeight), Title, ActualStyle, ContextSettings);

		GLCHECK();

		//If it failed to open, return an error
		if(!Window.isOpen())
		{
			Log::Instance.LogInfo(TAG, "Unable to create the rendering window.");

			return false;
		}

#if !FLPLATFORM_ANDROID
		Window.setPosition(sf::Vector2i((int32)WorkArea.Left, (int32)WorkArea.Top));
#endif

		ContextSettings = Window.getSettings();

		RenderCaps.DepthBits = ContextSettings.depthBits;
		RenderCaps.StencilBits = ContextSettings.stencilBits;
		RenderCaps.AntialiasLevel = ContextSettings.antialiasingLevel;

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
		glClearColor(Renderer::DefaultClearColor.x, Renderer::DefaultClearColor.y, Renderer::DefaultClearColor.z, Renderer::DefaultClearColor.w);

		return true;
	}

	FrameBufferHandle SFMLRendererImplementation::CreateFrameBuffer(const FrameBufferCreationInfo &Info)
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

	bool SFMLRendererImplementation::IsFrameBufferValid(FrameBufferHandle Handle)
	{
		FrameBufferMap::iterator it = FrameBuffers.find(Handle);

		return it != FrameBuffers.end();
	}

	void SFMLRendererImplementation::BindFrameBuffer(FrameBufferHandle Handle)
	{
		if (!IsFrameBufferValid(Handle))
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffers[Handle]->GLID);
	}

	void SFMLRendererImplementation::DestroyFrameBuffer(FrameBufferHandle Handle)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (!IsFrameBufferValid(Handle))
			return;

		FrameBufferMap::iterator it = FrameBuffers.find(Handle);

		glDeleteRenderbuffers(1, &it->second->RenderBufferID);
		glDeleteFramebuffers(1, &it->second->GLID);

		FrameBuffers.erase(it);
	}

	const RendererCapabilities &SFMLRendererImplementation::Capabilities() const
	{
		return RenderCaps;
	}

	RendererDisplayMode SFMLRendererImplementation::DesktopDisplayMode()
	{
		sf::VideoMode Mode = sf::VideoMode::getDesktopMode();

		RendererDisplayMode Out;

		Out.Width = Mode.width;
		Out.Height = Mode.height;
		Out.Bpp = Mode.bitsPerPixel;

		return Out;
	}

	std::vector<RendererDisplayMode> SFMLRendererImplementation::DisplayModes()
	{
		const std::vector<sf::VideoMode> &Modes = sf::VideoMode::getFullscreenModes();
		std::vector<RendererDisplayMode> Out;

		for(sf::VideoMode Mode : Modes)
		{
			if(Mode.bitsPerPixel != 32)
				continue;

			RendererDisplayMode OutMode;
			OutMode.Width = Mode.width;
			OutMode.Height = Mode.height;
			OutMode.Bpp = Mode.bitsPerPixel;

			Out.push_back(OutMode);
		}

		return Out;
	}

	Vector2 SFMLRendererImplementation::Size() const
	{
		return Vector2((f32)Window.getSize().x, (f32)Window.getSize().y);
	}

	VertexBufferHandle SFMLRendererImplementation::CreateVertexBuffer()
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

	void SFMLRendererImplementation::SetVertexBufferData(VertexBufferHandle Handle, uint8 DetailsMode, VertexElementDescriptor *Elements, uint32 ElementCount, const void *Data, uint32 DataByteSize)
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

	bool SFMLRendererImplementation::IsVertexBufferHandleValid(VertexBufferHandle Handle)
	{
		return Handle != 0 && VertexBuffers.find(Handle) != VertexBuffers.end();
	}

	void SFMLRendererImplementation::DestroyVertexBuffer(VertexBufferHandle Handle)
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

	void SFMLRendererImplementation::RenderVertices(uint32 VertexMode, VertexBufferHandle Buffer, uint32 Start, uint32 End)
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

	void SFMLRendererImplementation::SetClipRect(const Rect &ClippingRect)
	{
		FrameStatsValue.StateChanges++;
		FrameStatsValue.ClippingChanges++;

		EnableState(GL_SCISSOR_TEST);

		GLint x = (GLint)ClippingRect.Left, y = (GLint)(Size().y - ClippingRect.Bottom);

		GLsizei Width = (GLsizei)(ClippingRect.Right - ClippingRect.Left), Height = (GLsizei)(ClippingRect.Bottom - ClippingRect.Top);

		glScissor(x, y, Width, Height);

		GLCHECK();
	}

	void SFMLRendererImplementation::Clear(uint32 Buffers)
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

	void SFMLRendererImplementation::Display()
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

		//Android workaround for black screen
		Window.pushGLStates();
		Window.display();
		Window.popGLStates();

		TextureHandle PreviousTexture = LastBoundTexture;

		BindTexture((TextureHandle)0);

		BindTexture(PreviousTexture);
	}

	const RendererFrameStats &SFMLRendererImplementation::FrameStats() const
	{
		return PreviousFrameStatsValue;
	}

	void SFMLRendererImplementation::SetWorldMatrix(const Matrix4x4 &WorldMatrix)
	{
		SpriteCache::Instance.Flush(Target);

		FrameStatsValue.StateChanges++;
		FrameStatsValue.MatrixChanges++;

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&WorldMatrix.m[0][0]);

		GLCHECK();

		LastWorldMatrix = WorldMatrix;
	}

	void SFMLRendererImplementation::SetProjectionMatrix(const Matrix4x4 &ProjectionMatrix)
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

	void SFMLRendererImplementation::SetViewport(f32 x, f32 y, f32 Width, f32 Height)
	{
		FrameStatsValue.StateChanges++;

		glViewport((GLint)x, (GLint)y, (GLsizei)Width, (GLsizei)Height);

		GLCHECK();
	}

	TextureHandle SFMLRendererImplementation::CreateTexture()
	{
		FrameStatsValue.StateChanges++;

		uint32 GLID = 0;

		glGenTextures(1, (GLuint *)&GLID);
		GLCHECK();

		TextureCounter++;

		Textures[TextureCounter].GLID = GLID;

		return TextureCounter;
	}

	bool SFMLRendererImplementation::IsTextureHandleValid(TextureHandle Handle)
	{
		return Handle != 0 && Textures.find(Handle) != Textures.end();
	}

	void SFMLRendererImplementation::DestroyTexture(TextureHandle Handle)
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

	void SFMLRendererImplementation::SetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 Width, uint32 Height)
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

	void SFMLRendererImplementation::SetTextureWrapMode(TextureHandle Handle, uint32 WrapMode)
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

	void SFMLRendererImplementation::SetTextureFiltering(TextureHandle Handle, uint32 Filter)
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

	bool SFMLRendererImplementation::CaptureScreen(uint8 *Pixels, uint32 BufferByteCount)
	{
		if(BufferByteCount != (uint32)(Window.getSize().x * Window.getSize().y * 4))
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

	bool SFMLRendererImplementation::GetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 BufferByteCount)
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

	void SFMLRendererImplementation::SetBlendingMode(uint32 BlendingMode)
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

	void SFMLRendererImplementation::BindTexture(TextureHandle Handle)
	{
		if(Handle == LastBoundTexture)
			return;

		FrameStatsValue.TextureChanges++;

		LastBoundTexture = Handle;

		TextureMap::iterator it = Textures.find(Handle);

		glBindTexture(GL_TEXTURE_2D, it != Textures.end() ? it->second.GLID : 0);

		GLCHECK();
	}

	bool SFMLRendererImplementation::PollEvent(RendererEvent &Out)
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

		static sf::Event Event;

		if(Window.pollEvent(Event))
		{
			switch(Event.type)
			{
			case sf::Event::JoystickButtonPressed:
				Out.Type = RendererEventType::JoystickButtonPressed;
				Out.JoystickIndex = Event.joystickButton.joystickId;
				Out.JoystickButtonIndex = Event.joystickButton.button;

				return true;

			case sf::Event::JoystickButtonReleased:
				Out.Type = RendererEventType::JoystickButtonReleased;
				Out.JoystickIndex = Event.joystickButton.joystickId;
				Out.JoystickButtonIndex = Event.joystickButton.button;

				return true;

			case sf::Event::JoystickConnected:
				Out.Type = RendererEventType::JoystickConnected;
				Out.JoystickIndex = Event.joystickButton.joystickId;

				return true;

			case sf::Event::JoystickDisconnected:
				Out.Type = RendererEventType::JoystickDisconnected;
				Out.JoystickIndex = Event.joystickButton.joystickId;

				return true;

			case sf::Event::JoystickMoved:
				Out.Type = RendererEventType::JoystickAxisMoved;
				Out.JoystickIndex = Event.joystickMove.joystickId;
				Out.JoystickAxisIndex = Event.joystickMove.axis;

				if(fabs(Event.joystickMove.position) > JOYSTICKDEADZONE)
				{
					Out.JoystickAxisPosition = Event.joystickMove.position / 100.f;
				}
				else
				{
					Out.JoystickAxisPosition = 0;
				}

				return true;

			case sf::Event::MouseLeft:
				Out.Type = RendererEventType::MouseLeft;

				return true;

			case sf::Event::MouseEntered:
				Out.Type = RendererEventType::MouseEntered;

				return true;

			case sf::Event::KeyPressed:
				if(Event.key.code == -1)
					break;

				Out.Type = RendererEventType::KeyPressed;
				Out.KeyCode = Event.key.code;

				return true;

				break;
			case sf::Event::KeyReleased:
				if(Event.key.code == -1)
					break;

				Out.Type = RendererEventType::KeyReleased;
				Out.KeyCode = Event.key.code;

				return true;

			case sf::Event::MouseMoved:
				if (PlatformInfo::PlatformType == PlatformType::Mobile)
				{
					for (uint32 i = 0; i < InputMouseButton::Count; i++)
					{
						if (RendererManager::Instance.Input.MouseButtons[i].Pressed)
						{
							Out.Type = RendererEventType::TouchDrag;
							Out.TouchIndex = i;
							Out.TouchPosition = Vector2((f32)Event.mouseMove.x, (f32)Event.mouseMove.y);

							break;
						}
					}
				}
				else
				{
					Out.Type = RendererEventType::MouseMoved;
					Out.MousePosition = Vector2((f32)Event.mouseMove.x, (f32)Event.mouseMove.y);
				}

				return true;

			case sf::Event::MouseWheelMoved:
				if (PlatformInfo::PlatformType == PlatformType::PC)
				{
					Out.Type = RendererEventType::MouseDeltaMoved;
					Out.MouseDelta = (f32)Event.mouseWheel.delta;
				}

				return true;

			case sf::Event::MouseButtonPressed:
				if (PlatformInfo::PlatformType == PlatformType::Mobile)
				{
					Out.Type = RendererEventType::TouchDown;
					Out.TouchIndex = Event.mouseButton.button;
					Out.TouchPosition = Vector2((f32)Event.mouseButton.x, (f32)Event.mouseButton.y);
				}
				else
				{
					Out.Type = RendererEventType::MouseButtonPressed;
					Out.MouseButtonIndex = Event.mouseButton.button;
				}

				return true;

			case sf::Event::MouseButtonReleased:
				if (PlatformInfo::PlatformType == PlatformType::Mobile)
				{
					Out.Type = RendererEventType::TouchUp;
					Out.TouchIndex = Event.mouseButton.button;
					Out.TouchPosition = Vector2((f32)Event.mouseButton.x, (f32)Event.mouseButton.y);
				}
				else
				{
					Out.Type = RendererEventType::MouseButtonReleased;
					Out.MouseButtonIndex = Event.mouseButton.button;
				}

				return true;

			case sf::Event::Closed:
				Out.Type = RendererEventType::WindowClosed;

				return true;

			case sf::Event::TextEntered:
				Out.Type = RendererEventType::CharacterEntered;
				Out.TypedCharacter = Event.text.unicode;

				return true;

			case sf::Event::Resized:
				Out.Type = RendererEventType::WindowResized;
				Out.WindowSize = Vector2((f32)Event.size.width, (f32)Event.size.height);

				{
					sf::View TheView(sf::FloatRect(0, 0, Out.WindowSize.x, Out.WindowSize.y));
					Window.setView(TheView);
				}

				Target->OnResized(Target, (uint32)Out.WindowSize.x, (uint32)Out.WindowSize.y);

				return true;

			case sf::Event::GainedFocus:
				Out.Type = RendererEventType::WindowGotFocus;

				return true;

			case sf::Event::LostFocus:
				Out.Type = RendererEventType::WindowLostFocus;

				return true;

			case sf::Event::TouchBegan:
				Out.Type = RendererEventType::TouchDown;
				Out.TouchIndex = Event.touch.finger;
				Out.TouchPosition = Vector2((f32)Event.touch.x, (f32)Event.touch.y);

				return true;

			case sf::Event::TouchEnded:
				Out.Type = RendererEventType::TouchUp;
				Out.TouchIndex = Event.touch.finger;
				Out.TouchPosition = Vector2((f32)Event.touch.x, (f32)Event.touch.y);

				return true;

			case sf::Event::TouchMoved:
				Out.Type = RendererEventType::TouchDrag;
				Out.TouchIndex = Event.touch.finger;
				Out.TouchPosition = Vector2((f32)Event.touch.x, (f32)Event.touch.y);

				return true;
			}
		}

		return false;
	}

	void SFMLRendererImplementation::SetMousePosition(const Vector2 &Position)
	{
		sf::Mouse::setPosition(sf::Vector2i((int32)Position.x, (int32)Position.y), Window);
	}

	void *SFMLRendererImplementation::WindowHandle() const
	{
		return (void *)Window.getSystemHandle();
	}

	void SFMLRendererImplementation::SetFrameRate(uint32 FPS)
	{
		Window.setFramerateLimit(FPS);
	}

	bool SFMLRendererImplementation::IsStateEnabled(uint32 ID) const
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

	void SFMLRendererImplementation::EnableState(uint32 ID)
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

	void SFMLRendererImplementation::DisableState(uint32 ID)
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

	void SFMLRendererImplementation::ReportSkippedDrawCall()
	{
		FrameStatsValue.SkippedDrawCalls++;
	}

#	endif
}
