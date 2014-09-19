#include "FlamingCore.hpp"

namespace FlamingTorch
{
#	include "SFMLRenderer.hpp"
#	define TAG "SFMLRenderer"

	bool SFMLRendererImplementation::FirstRenderer = true;
	uint64 SFMLRendererImplementation::TextureCounter = 0;
	uint64 SFMLRendererImplementation::FontCounter = 0;
	uint64 SFMLRendererImplementation::VertexBufferCounter = 0;

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

		int t, t2, t3;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &t);

		GLCHECK();

		Log::Instance.LogInfo(TAG, "   Max Texture Size: %d", t);

		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &t);
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &t2);
		glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &t3);

		GLCHECK();

		Log::Instance.LogInfo(TAG, "   Max Texture Units: %d (%d vs, %d ps)", t, t3, t2);

		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &t);

		GLCHECK();

		Log::Instance.LogInfo(TAG, "   Max Vertex Attribs: %d", t);
	};

	SFMLRendererImplementation::SFMLRendererImplementation() : LastBoundTexture(0)
	{
	};

	SFMLRendererImplementation::~SFMLRendererImplementation()
	{
		while(Fonts.size())
		{
			Fonts.begin()->second.ActualFont.Dispose();
			Fonts.erase(Fonts.begin());
		};

		while(Textures.size())
		{
			DestroyTexture(Textures.begin()->first);
		};
	};

	bool SFMLRendererImplementation::Create(void *WindowHandle)
	{
		//Init from an existing window
		Window.create((sf::WindowHandle)WindowHandle);

		GLCHECK();

		//If failed to open, return an error
		if(!Window.isOpen())
		{
			Log::Instance.LogInfo(TAG, "Unable to create the rendering window.");

			return 0;
		};

		//If the first renderer, init glew
		if(FirstRenderer)
		{
			FirstRenderer = false;

			GLenum err = glewInit();

			if(GLEW_OK != err)
			{
				Log::Instance.LogInfo(TAG, "GLEW failed to start, so no fancy OpenGL extensions will be available. Error Message: %s",
					glewGetErrorString(err));
			};

			GLCHECK();

			DumpRendererStats();
		};

		return true;
	};

	bool SFMLRendererImplementation::Create(const std::string &Title, uint32 Width, uint32 Height, uint32 Style)
	{
		//Create the window
		Window.create(sf::VideoMode(Width, Height), Title, Style);

		GLCHECK();

		//If it failed to open, return an error
		if(!Window.isOpen())
		{
			Log::Instance.LogInfo(TAG, "Unable to create the rendering window.");

			return false;
		};

		//If the first renderer, init glew
		if(FirstRenderer)
		{
			FirstRenderer = false;

			GLenum err = glewInit();

			if(GLEW_OK != err)
			{
				Log::Instance.LogInfo(TAG, "GLEW failed to start, so no fancy OpenGL extensions will be available. Error Message: %s",
					glewGetErrorString(err));
			};

			GLCHECK();

			DumpRendererStats();
		};

		return true;
	};

	Vector2 SFMLRendererImplementation::Size() const
	{
		return Vector2((f32)Window.getSize().x, (f32)Window.getSize().y);
	};

	VertexBufferHandle SFMLRendererImplementation::CreateVertexBuffer()
	{
		VertexBufferCounter++;

		VertexBufferInfo &Info = VertexBuffers[VertexBufferCounter];
		Info.VertexCount = 0;
		Info.VertexSize = 0;

		return VertexBufferCounter;
	};

	void SFMLRendererImplementation::SetVertexBufferData(VertexBufferHandle Handle, uint8 DetailsMode, VertexElementDescriptor *Elements, uint32 ElementCount, const void *Data, uint32 DataByteSize)
	{
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
			};

			if(Elements[i].DataType >= VertexElementDataType::Count)
			{
				Log::Instance.LogErr(TAG, "Unknown data type '%d'", Elements[i].DataType);

				return;
			};

			switch(Elements[i].Type)
			{
			case VertexElementType::Position:
				if(PositionOffset != -1)
				{
					Log::Instance.LogErr(TAG, "Found duplicate element for POSITION");

					return;
				};

				PositionOffset = Elements[i].Offset;
				PositionDataType = Elements[i].DataType;
				PositionDataSize = VertexBufferDataElementSizes[Elements[i].DataType];

				break;
			case VertexElementType::TexCoord:
				if(TexCoordOffset != -1)
				{
					Log::Instance.LogErr(TAG, "Found duplicate element for TEXCOORD");

					return;
				};

				TexCoordOffset = Elements[i].Offset;
				TexCoordDataType = Elements[i].DataType;
				TexCoordDataSize = VertexBufferDataElementSizes[Elements[i].DataType];

				break;

			case VertexElementType::Normal:
				if(NormalOffset != -1)
				{
					Log::Instance.LogErr(TAG, "Found duplicate element for NORMAL");

					return;
				};

				NormalOffset = Elements[i].Offset;
				NormalDataType = Elements[i].DataType;
				NormalDataSize = VertexBufferDataElementSizes[Elements[i].DataType];

				break;

			case VertexElementType::Color:
				if(ColorOffset != -1)
				{
					Log::Instance.LogErr(TAG, "Found duplicate element for COLOR");

					return;
				};

				ColorOffset = Elements[i].Offset;
				ColorDataType = Elements[i].DataType;
				ColorDataSize = VertexBufferDataElementSizes[Elements[i].DataType];

				break;

			default:
				Log::Instance.LogErr(TAG, "Unknown element type '%d'", Elements[i].Type);

				return;
			};

			LastOffset = Elements[i].Offset;
			LastElementSize = VertexBufferDataElementSizes[Elements[i].DataType];
		};

		if(PositionOffset == -1)
		{
			Log::Instance.LogErr(TAG, "Expected a Position in this buffer");

			return;
		};

		VertexSize = (uint32)MathUtils::Max((f32)VertexSize, (f32)PositionOffset + PositionDataSize);
		VertexSize = (uint32)MathUtils::Max((f32)VertexSize, (f32)TexCoordOffset + TexCoordDataSize);
		VertexSize = (uint32)MathUtils::Max((f32)VertexSize, (f32)ColorOffset + ColorDataSize);
		VertexSize = (uint32)MathUtils::Max((f32)VertexSize, (f32)NormalOffset + NormalDataSize);

		if(DataByteSize % VertexSize != 0)
		{
			Log::Instance.LogErr(TAG, "Data Byte Size is incompatible with estimated Vertex Size '%d': Not a multiple of Vertex Size.", VertexSize);

			return;
		};

		uint32 VertexCount = DataByteSize / VertexSize;

		it->second.VertexCount = VertexCount;
		it->second.VertexSize = VertexSize;
		it->second.PositionDataType = PositionDataType;
		it->second.TexCoordDataType = TexCoordDataType;
		it->second.NormalsDataType = NormalDataType;
		it->second.ColorDataType = ColorDataType;

		it->second.PositionData.resize(VertexCount * PositionDataSize);

		if(TexCoordDataSize)
		{
			it->second.TexCoordData.resize(VertexCount * TexCoordDataSize);
		}
		else
		{
			it->second.TexCoordData.resize(0);
		};

		if(ColorDataSize)
		{
			it->second.ColorData.resize(VertexCount * ColorDataSize);
		}
		else
		{
			it->second.ColorData.resize(0);
		};

		if(NormalDataSize)
		{
			it->second.NormalsData.resize(VertexCount * NormalDataSize);
		}
		else
		{
			it->second.NormalsData.resize(0);
		};

		switch(DetailsMode)
		{
		case VertexDetailsMode::Lists:
			memcpy(&it->second.PositionData[0], &((uint8 *)Data)[PositionOffset], PositionDataSize * VertexCount);

			if(TexCoordDataSize)
			{
				memcpy(&it->second.TexCoordData[0], &((uint8 *)Data)[TexCoordOffset], TexCoordDataSize * VertexCount);
			};

			if(ColorDataSize)
			{
				memcpy(&it->second.ColorData[0], &((uint8 *)Data)[ColorOffset], ColorDataSize * VertexCount);
			};

			if(NormalDataSize)
			{
				memcpy(&it->second.NormalsData[0], &((uint8 *)Data)[NormalOffset], NormalDataSize * VertexCount);
			};

			break;

		case VertexDetailsMode::Mixed:
			{
				uint32 PositionByteOffset = 0, TexCoordByteOffset = 0, NormalByteOffset = 0, ColorByteOffset = 0;

				for(uint32 i = 0; i < DataByteSize;)
				{
					for(uint32 j = 0; j < ElementCount; j++)
					{
						switch(Elements[j].Type)
						{
						case VertexElementType::Position:
							memcpy(&it->second.PositionData[PositionByteOffset], &((uint8 *)Data)[i], PositionDataSize);

							i += PositionDataSize;
							PositionByteOffset += PositionDataSize;

							break;

						case VertexElementType::Color:
							memcpy(&it->second.ColorData[ColorByteOffset], &((uint8 *)Data)[i], ColorDataSize);

							i += ColorDataSize;
							ColorByteOffset += ColorDataSize;

							break;

						case VertexElementType::TexCoord:
							memcpy(&it->second.TexCoordData[TexCoordByteOffset], &((uint8 *)Data)[i], TexCoordDataSize);

							i += TexCoordDataSize;
							TexCoordByteOffset += TexCoordDataSize;

							break;

						case VertexElementType::Normal:
							memcpy(&it->second.NormalsData[NormalByteOffset], &((uint8 *)Data)[i], NormalDataSize);

							i += NormalDataSize;
							NormalByteOffset += NormalDataSize;

							break;
						};
					};
				};
			};

			break;
		};
	};

	bool SFMLRendererImplementation::IsVertexBufferHandleValid(VertexBufferHandle Handle)
	{
		return Handle != 0 && VertexBuffers.find(Handle) != VertexBuffers.end();
	};

	void SFMLRendererImplementation::DestroyVertexBuffer(VertexBufferHandle Handle)
	{
		VertexBufferMap::iterator it = VertexBuffers.find(Handle);

		if(it == VertexBuffers.end())
			return;

		VertexBuffers.erase(it);
	};

	void SFMLRendererImplementation::RenderVertices(uint32 VertexMode, VertexBufferHandle Buffer, uint32 Start, uint32 End)
	{
		VertexBufferMap::iterator it = VertexBuffers.find(Buffer);

		if(it == VertexBuffers.end())
		{
			Log::Instance.LogErr(TAG, "Unable to render a buffer '%d': Invalid Buffer", Buffer);

			return;
		};

		if(VertexMode >= VertexModes::Count)
		{
			Log::Instance.LogErr(TAG, "Unable to render a buffer '%d': Invalid Vertex Mode '%d'", Buffer, VertexMode);

			return;
		};

		if(End <= Start)
		{
			Log::Instance.LogErr(TAG, "Unable to render a buffer '%d': Invalid Start/End Pair '%d, %d'", Buffer, Start, End);

			return;
		};

		if(LastBoundTexture != 0)
		{
			EnableState(GL_TEXTURE_2D);
		}
		else
		{
			DisableState(GL_TEXTURE_2D);
		};

		EnableState(GL_VERTEX_ARRAY);

		glVertexPointer(VertexBufferDataElementCount[it->second.PositionDataType], GL_FLOAT, 0, &it->second.PositionData[0]);

		if(it->second.TexCoordData.size())
		{
			EnableState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(VertexBufferDataElementCount[it->second.TexCoordDataType], GL_FLOAT, 0, &it->second.TexCoordData[0]);
		}
		else
		{
			DisableState(GL_TEXTURE_COORD_ARRAY);
		};

		if(it->second.ColorData.size())
		{
			EnableState(GL_COLOR_ARRAY);
			glColorPointer(VertexBufferDataElementCount[it->second.ColorDataType], GL_FLOAT, 0, &it->second.ColorData[0]);
		}
		else
		{
			DisableState(GL_COLOR_ARRAY);
			glColor4f(1, 1, 1, 1);
		};

		if(it->second.NormalsData.size())
		{
			EnableState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, 0, &it->second.NormalsData[0]);
		}
		else
		{
			DisableState(GL_NORMAL_ARRAY);
		};

		glDrawArrays(VertexBufferVertexMode[VertexMode], Start, End - Start);
	};

	void SFMLRendererImplementation::StartClipping(const Rect &ClippingRect)
	{
		FLASSERT(ClippingRect.Bottom > ClippingRect.Top, "Expected a larger Bottom coordinate on ClippingRect");
		FLASSERT(ClippingRect.Right > ClippingRect.Left, "Expected a larger Right coordinate on ClippingRect");

		if(ClippingRect.Bottom <= ClippingRect.Top || ClippingRect.Right <= ClippingRect.Left)
			return;

		ClippingStack.push_back(ClippingRect);

		EnableState(GL_SCISSOR_TEST);

		glScissor(ClippingRect.Left, ClippingRect.Bottom, ClippingRect.Right - ClippingRect.Left, ClippingRect.Bottom - ClippingRect.Top);
	};

	void SFMLRendererImplementation::FinishClipping()
	{
		if(!ClippingStack.size())
			return;

		ClippingStack.pop_back();

		if(ClippingStack.size())
		{
			const Rect &ClippingRect = ClippingStack.back();

			glScissor(ClippingRect.Left, ClippingRect.Bottom, ClippingRect.Right - ClippingRect.Left, ClippingRect.Bottom - ClippingRect.Top);
		}
		else
		{
			DisableState(GL_SCISSOR_TEST);
		};
	};

	void SFMLRendererImplementation::Clear(uint32 Buffers)
	{
		uint32 Mask = 0;

		if(Buffers & RendererBuffer::Color)
			Mask |= GL_COLOR_BUFFER_BIT;

		if(Buffers & RendererBuffer::Depth)
			Mask |= GL_DEPTH_BUFFER_BIT;

		if(Buffers & RendererBuffer::Stencil)
			Mask |= GL_STENCIL_BUFFER_BIT;

		if(Mask != 0)
			glClear(Mask);
	};

	void SFMLRendererImplementation::Display()
	{
		Window.display();
	};

	void SFMLRendererImplementation::SetWorldMatrix(const Matrix4x4 &WorldMatrix)
	{
		LastWorldMatrix = WorldMatrix;

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&WorldMatrix.m[0][0]);
	};

	void SFMLRendererImplementation::SetProjectionMatrix(const Matrix4x4 &ProjectionMatrix)
	{
		LastProjectionMatrix = ProjectionMatrix;

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(&ProjectionMatrix.m[0][0]);
		glMatrixMode(GL_MODELVIEW);
	};

	void SFMLRendererImplementation::PushMatrices()
	{
		MatrixStackElement Element;
		Element.World = LastWorldMatrix;
		Element.Projection = LastProjectionMatrix;

		MatrixStack.push_back(Element);
	};

	void SFMLRendererImplementation::PopMatrices()
	{
		if(MatrixStack.size() == 0)
			return;

		const MatrixStackElement &Element = MatrixStack.back();

		SetWorldMatrix(Element.World);
		SetProjectionMatrix(Element.Projection);

		MatrixStack.pop_back();
	};

	const Matrix4x4 &SFMLRendererImplementation::WorldMatrix()
	{
		return LastWorldMatrix;
	};

	const Matrix4x4 &SFMLRendererImplementation::ProjectionMatrix()
	{
		return LastProjectionMatrix;
	};

	void SFMLRendererImplementation::SetViewport(f32 x, f32 y, f32 Width, f32 Height)
	{
		glViewport((GLint)x, (GLint)y, (GLsizei)Width, (GLsizei)Height);
	};

	TextureHandle SFMLRendererImplementation::CreateTexture()
	{
		uint32 GLID = 0;

		glGenTextures(1, (GLuint *)&GLID);
		GLCHECK();

		TextureCounter++;

		Textures[TextureCounter].GLID = GLID;

		return TextureCounter;
	};

	bool SFMLRendererImplementation::IsTextureHandleValid(TextureHandle Handle)
	{
		return Handle != 0 && Textures.find(Handle) != Textures.end();
	};

	void SFMLRendererImplementation::DestroyTexture(TextureHandle Handle)
	{
		if(Handle == 0)
			return;

		TextureMap::iterator it = Textures.find(Handle);

		if(it == Textures.end())
			return;

		glDeleteTextures(1, (GLuint *)&it->second.GLID);

		GLCHECK();

		Textures.erase(it);
	};

	void SFMLRendererImplementation::SetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 Width, uint32 Height)
	{
		TextureHandle Last = LastBoundTexture;

		BindTexture(Handle);

		GLCHECK();

		if(!IsTextureHandleValid(Handle))
		{
			if(Last)
				BindTexture(Last);

			return;
		};

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Pixels);

		GLCHECK();

		TextureInfo &Info = Textures[Handle];

		Info.Width = Width;
		Info.Height = Height;

		BindTexture(Last);

		GLCHECK();
	};

	void SFMLRendererImplementation::SetTextureWrapMode(TextureHandle Handle, uint32 WrapMode)
	{
		TextureHandle Last = LastBoundTexture;

		BindTexture(Handle);

		GLCHECK();
		
		if(!IsTextureHandleValid(Handle))
		{
			if(Last)
				BindTexture(Last);

			return;
		};

		switch(WrapMode)
		{
		case TextureWrapMode::Repeat:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			break;

		case TextureWrapMode::Clamp:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			break;

		case TextureWrapMode::ClampToBorder:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

			break;

		case TextureWrapMode::ClampToEdge:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			break;
		};

		GLCHECK();

		BindTexture(Last);

		GLCHECK();
	};

	void SFMLRendererImplementation::SetTextureFiltering(TextureHandle Handle, uint32 Filter)
	{
		TextureHandle Last = LastBoundTexture;

		BindTexture(Handle);

		GLCHECK();

		if(!IsTextureHandleValid(Handle))
		{
			if(Last)
				BindTexture(Last);

			return;
		};

		if(Filter == TextureFiltering::Nearest || Filter == TextureFiltering::Linear)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter == TextureFiltering::Nearest ?
GL_NEAREST : GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter == TextureFiltering::Nearest ?
GL_NEAREST : GL_LINEAR);
		}
		else if(Filter == TextureFiltering::Nearest_Mipmap || Filter == TextureFiltering::Linear_Mipmap)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter == TextureFiltering::Nearest_Mipmap ?
GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter == TextureFiltering::Nearest_Mipmap ?
GL_NEAREST : GL_LINEAR);

			GLCHECK();

			glGenerateMipmap(GL_TEXTURE_2D);

			GLCHECK();
		};

		BindTexture(Last);

		GLCHECK();
	};

	bool SFMLRendererImplementation::CaptureScreen(uint8 *Pixels, uint32 BufferByteCount)
	{
		if(BufferByteCount != (uint32)(Window.getSize().x * Window.getSize().y * 4))
			return false;

		sf::Image Picture = Window.capture();

		memcpy(Pixels, Picture.getPixelsPtr(), BufferByteCount);

		return true;
	};

	bool SFMLRendererImplementation::GetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 BufferByteCount)
	{
		TextureHandle Last = LastBoundTexture;

		BindTexture(Handle);

		GLCHECK();

		if(!IsTextureHandleValid(Handle))
		{
			if(Last)
				BindTexture(Last);

			return false;
		};

		TextureInfo &Info = Textures[Handle];

		if(BufferByteCount != Info.Width * Info.Height * 4)
		{
			if(Last)
				BindTexture(Last);

			return false;
		};

		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA8, GL_UNSIGNED_BYTE, Pixels);

		GLCHECK();

		BindTexture(Last);

		GLCHECK();

		return true;
	};

	void SFMLRendererImplementation::SetBlendingMode(uint32 BlendingMode)
	{
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
			glBlendEquation(GL_FUNC_SUBTRACT);

			break;
		};
	};

	void SFMLRendererImplementation::BindTexture(TextureHandle Handle)
	{
		LastBoundTexture = Handle;

		TextureMap::iterator it = Textures.find(Handle);

		glBindTexture(GL_TEXTURE_2D, it != Textures.end() ? it->second.GLID : 0);

		GLCHECK();
	};

	bool SFMLRendererImplementation::PollEvent(RendererEvent &Out)
	{
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

				break;

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
				};

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
				Out.Type = RendererEventType::MouseMoved;
				Out.MousePosition = Vector2((f32)Event.mouseMove.x, (f32)Event.mouseMove.y);

				return true;

			case sf::Event::MouseWheelMoved:
				Out.Type = RendererEventType::MouseDeltaMoved;
				Out.MouseDelta = (f32)Event.mouseWheel.delta;

				return true;

			case sf::Event::MouseButtonPressed:
				Out.Type = RendererEventType::MouseButtonPressed;
				Out.MouseButtonIndex = Event.mouseButton.button;

				return true;

			case sf::Event::MouseButtonReleased:
				Out.Type = RendererEventType::MouseButtonReleased;
				Out.MouseButtonIndex = Event.mouseButton.button;

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
				};

				Target->OnResized(Target, (uint32)Out.WindowSize.x, (uint32)Out.WindowSize.y);

				return true;

			case sf::Event::GainedFocus:
				Out.Type = RendererEventType::WindowGotFocus;

				return true;

			case sf::Event::LostFocus:
				Out.Type = RendererEventType::WindowLostFocus;

				return true;
			};
		};

		return false;
	};

	void SFMLRendererImplementation::SetMousePosition(const Vector2 &Position)
	{
		sf::Mouse::setPosition(sf::Vector2i((int32)Position.x, (int32)Position.y), Window);
	};

	void *SFMLRendererImplementation::WindowHandle() const
	{
		return Window.getSystemHandle();
	};

	void SFMLRendererImplementation::SetFrameRate(uint32 FPS)
	{
		Window.setFramerateLimit(FPS);
	};

	FontHandle SFMLRendererImplementation::CreateFont(Stream *Data)
	{
		FontCounter++;

		FontInfo &TheFont = Fonts[FontCounter];

		TheFont.ActualFont.Reset(new sf::Font());
		TheFont.Data.resize((uint32)Data->Length());

		Data->AsBuffer(&TheFont.Data[0], (uint32)Data->Length());

		if(!TheFont.ActualFont->loadFromMemory(&TheFont.Data[0], TheFont.Data.size()))
		{
			Log::Instance.LogErr(TAG, "Failed to create a font from a stream '%s' of size '%d'", StringUtils::PointerString(Data), Data->Length());

			Fonts.erase(FontCounter);

			return 0;
		};

		return FontCounter;
	};

	void SFMLRendererImplementation::DestroyFont(FontHandle Handle)
	{
		FontMap::iterator it = Fonts.find(Handle);

		if(it != Fonts.end())
			Fonts.erase(it);
	};

	Rect SFMLRendererImplementation::MeasureText(FontHandle Handle, const std::string &TheText, const TextParams &Parameters)
	{
		//Workaround for newline strings
		if(TheText.length() == 0)
			return Rect(0, 0, 0, (f32)Parameters.FontSizeValue);

		FontMap::iterator FontIterator = Fonts.find(Parameters.FontValue ? Parameters.FontValue : 0);

		if(FontIterator == Fonts.end())
			return Rect();

		uint32 Style = sf::Text::Regular;

		if(Parameters.StyleValue & TextStyle::Bold)
		{
			Style |= sf::Text::Bold;
		};

		if(Parameters.StyleValue & TextStyle::Italic)
		{
			Style |= sf::Text::Italic;
		};

		if(Parameters.StyleValue & TextStyle::Underline)
		{
			Style |= sf::Text::Underlined;
		};

		sf::Text Text;
		Text.setFont(*FontIterator->second.ActualFont);
		Text.setCharacterSize(Parameters.FontSizeValue);
		Text.setString(TheText);
		Text.setStyle(Style);

		sf::FloatRect InRect = Text.getLocalBounds();

		//Width/Height are considering the left/top position, we have to consider that too
		return Rect(InRect.left, InRect.width, InRect.top + InRect.left, InRect.height + InRect.top);
	};

	void SFMLRendererImplementation::RenderText(FontHandle Handle, const std::string &TheText, const TextParams &Parameters)
	{
		if(TheText.length() == 0)
			return;

		Vector2 ActualPosition = Parameters.PositionValue;

		FontMap::iterator FontIterator = Fonts.find(Parameters.FontValue ? Parameters.FontValue : 0);

		if(FontIterator == Fonts.end())
			return;
		
		GLCHECK();

		sf::Color Border((uint8)(Parameters.BorderColorValue.x * 255),
			(uint8)(Parameters.BorderColorValue.y * 255),
			(uint8)(Parameters.BorderColorValue.z * 255),
			(uint8)(Parameters.BorderColorValue.w * 255));

		sf::Color ActualTextColor((uint8)(Parameters.TextColorValue.x * 255),
			(uint8)(Parameters.TextColorValue.y * 255),
			(uint8)(Parameters.TextColorValue.z * 255),
			(uint8)(Parameters.TextColorValue.w * 255));

		sf::Color ActualTextColor2((uint8)(Parameters.SecondaryTextColorValue.x * 255),
			(uint8)(Parameters.SecondaryTextColorValue.y * 255),
			(uint8)(Parameters.SecondaryTextColorValue.z * 255),
			(uint8)(Parameters.SecondaryTextColorValue.w * 255));

		static sf::Text Text;

		uint32 Style = sf::Text::Regular;

		if(Parameters.StyleValue & TextStyle::Bold)
		{
			Style |= sf::Text::Bold;
		};

		if(Parameters.StyleValue & TextStyle::Italic)
		{
			Style |= sf::Text::Italic;
		};

		if(Parameters.StyleValue & TextStyle::Underline)
		{
			Style |= sf::Text::Underlined;
		};

		Text.setFont(*FontIterator->second.ActualFont);
		Text.setCharacterSize(Parameters.FontSizeValue);
		Text.setColor(ActualTextColor);
		Text.setColor2(ActualTextColor2);
		Text.setBorderColor(Border);
		Text.setBorderSize(Parameters.BorderSizeValue);
		Text.setString(TheText);
		Text.setStyle(Style);

		const sf::VertexArray &Vertices = Text.getVertices();

		uint32 VertexCount = Vertices.getVertexCount() / 4 * 6;
		const sf::Texture *FontTexture = &FontIterator->second.ActualFont->getTexture(Text.getCharacterSize(), Text.getColor(), Text.getColor2(), Text.getBorderSize(), Text.getBorderColor());
		Vector2 FontTextureSize((f32)FontTexture->getSize().x, (f32)FontTexture->getSize().y);

		static std::vector<Vector2> Positions(VertexCount), TexCoords(VertexCount);
		static std::vector<Vector4> Colors(VertexCount);

		Positions.resize(VertexCount);
		TexCoords.resize(VertexCount);
		Colors.resize(VertexCount);

		for(uint32 i = 0, index = 0; i < VertexCount; i+=6, index+=4)
		{
			Positions[i] = Positions[i + 5] = Vector2(Vertices[index].position.x, Vertices[index].position.y);
			Positions[i + 1] = Vector2(Vertices[index + 3].position.x, Vertices[index + 3].position.y);
			Positions[i + 2] = Positions[i + 3] = Vector2(Vertices[index + 2].position.x, Vertices[index + 2].position.y);
			Positions[i + 4] = Vector2(Vertices[index + 1].position.x, Vertices[index + 1].position.y);

			TexCoords[i] = TexCoords[i + 5] = Vector2(Vertices[index].texCoords.x, Vertices[index].texCoords.y) / FontTextureSize;
			TexCoords[i + 1] = Vector2(Vertices[index + 3].texCoords.x, Vertices[index + 3].texCoords.y) / FontTextureSize;
			TexCoords[i + 2] = TexCoords[i + 3] = Vector2(Vertices[index + 2].texCoords.x, Vertices[index + 2].texCoords.y) / FontTextureSize;
			TexCoords[i + 4] = Vector2(Vertices[index + 1].texCoords.x, Vertices[index + 1].texCoords.y) / FontTextureSize;

			Colors[i] = Colors[i + 5] = Vector4(Vertices[index].color.r / 255.f, Vertices[index].color.g / 255.f, Vertices[index].color.b / 255.f,
				Vertices[index].color.a / 255.f);
			Colors[i + 1] = Vector4(Vertices[index + 3].color.r / 255.f, Vertices[index + 3].color.g / 255.f, Vertices[index + 3].color.b / 255.f,
				Vertices[index + 3].color.a / 255.f);;
			Colors[i + 2] = Colors[i + 3] = Vector4(Vertices[index + 2].color.r / 255.f, Vertices[index + 2].color.g / 255.f, Vertices[index + 2].color.b / 255.f,
				Vertices[index + 2].color.a / 255.f);
			Colors[i + 4] = Vector4(Vertices[index + 1].color.r / 255.f, Vertices[index + 1].color.g / 255.f, Vertices[index + 1].color.b / 255.f,
				Vertices[index + 1].color.a / 255.f);
		};

		if(!Positions.size())
			return;

		sf::FloatRect ObjectRect = Text.getLocalBounds();

		Vector2 ObjectSize = Vector2(ObjectRect.left * 2 + ObjectRect.width, ObjectRect.top * 2 + ObjectRect.height) / 2;

		if(Parameters.RotationValue != 0)
		{
			for(uint32 i = 0; i < VertexCount; i++)
			{
				Positions[i] = Vector2::Rotate(Positions[i] - ObjectSize, Parameters.RotationValue) + ObjectSize + ActualPosition;
			};
		}
		else
		{
			for(uint32 i = 0; i < VertexCount; i++)
			{
				Positions[i] = Positions[i] + ActualPosition;
			};
		};

		SpriteCache::Instance.Flush(Target);

		BindTexture((TextureHandle)0);

		sf::Texture::bind(FontTexture);

		EnableState(GL_VERTEX_ARRAY);
		EnableState(GL_TEXTURE_COORD_ARRAY);
		EnableState(GL_COLOR_ARRAY);
		DisableState(GL_NORMAL_ARRAY);

		bool TextureWasEnabled = IsStateEnabled(GL_TEXTURE_2D);

		EnableState(GL_TEXTURE_2D);

		glVertexPointer(2, GL_FLOAT, 0, &Positions[0]);
		glTexCoordPointer(2, GL_FLOAT, 0, &TexCoords[0]);
		glColorPointer(4, GL_FLOAT, 0, &Colors[0]);

		glDrawArrays(GL_TRIANGLES, 0, Positions.size());

		glBindTexture(GL_TEXTURE_2D, 0);

		if(!TextureWasEnabled)
		{
			DisableState(GL_TEXTURE_2D);
		};
	};

	bool SFMLRendererImplementation::IsStateEnabled(uint32 ID) const
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

	void SFMLRendererImplementation::EnableState(uint32 ID)
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

	void SFMLRendererImplementation::DisableState(uint32 ID)
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
};