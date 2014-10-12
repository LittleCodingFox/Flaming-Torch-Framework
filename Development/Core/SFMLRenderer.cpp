#include "FlamingCore.hpp"

namespace FlamingTorch
{
#	if USE_SFML_RENDERER
#	include "SFMLRenderer.hpp"
#	define TAG "SFMLRenderer"
#	define GLCHECK() { int32 error = glGetError(); if(error != GL_NO_ERROR) { FLASSERT(0, "GL Error %08x!", error); } }
#	define ONE_MB_FLOAT 1048576.f
#	define BUFFER_OFFSET(x) ((char *)NULL + x)
#	define SFML_RENDERER_VERSION_MAJOR 0
#	define SFML_RENDERER_VERSION_MINOR 1
#	define USE_TEXT_CACHE 1

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

	SFMLRendererImplementation::SFMLRendererImplementation() : LastBoundTexture(0), SupportsVBOs(false), ExtensionsAvailable(false), LastBoundVBO(0), 
		UniqueCacheStringID(0), SavedTextDrawcalls(0)
	{
		FrameStatsValue.RendererName = "SFML";
		FrameStatsValue.RendererVersion = CoreUtils::MakeVersionString(SFML_RENDERER_VERSION_MAJOR, SFML_RENDERER_VERSION_MINOR);
		FrameStatsValue.RendererCustomMessage = "";
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

				ExtensionsAvailable = false;
			}
			else
			{
				ExtensionsAvailable = true;
				//Disabled for now
				//SupportsVBOs = !!glewIsSupported("GL_ARB_vertex_buffer_object");
			};

			GLCHECK();

			DumpRendererStats();
		};

		glDisable(GL_DEPTH_TEST);

		return true;
	};

	bool SFMLRendererImplementation::Create(const std::string &Title, uint32 Width, uint32 Height, uint32 Style)
	{
		uint32 ActualStyle = sf::Style::None;

		switch(Style)
		{
		case RendererWindowStyle::FullScreen:
			ActualStyle = sf::Style::Fullscreen;

			break;

		case RendererWindowStyle::Popup:
			ActualStyle = sf::Style::None;

			break;

		default:
			ActualStyle = sf::Style::Close;

			break;
		};

		//Create the window
		Window.create(sf::VideoMode(Width, Height), Title, ActualStyle);

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

				ExtensionsAvailable = false;
			}
			else
			{
				ExtensionsAvailable = true;
				//Disabled for now
				//SupportsVBOs = !!glewIsSupported("GL_ARB_vertex_buffer_object");
			};

			GLCHECK();

			DumpRendererStats();
		};

		glDisable(GL_DEPTH_TEST);

		return true;
	};

	Vector2 SFMLRendererImplementation::Size() const
	{
		return Vector2((f32)Window.getSize().x, (f32)Window.getSize().y);
	};

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
		};

		return VertexBufferCounter;
	};

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
		};

		if(ColorDataSize)
		{
			Info.ColorData.resize(VertexCount * ColorDataSize);
		}
		else
		{
			Info.ColorData.resize(0);
		};

		if(NormalDataSize)
		{
			Info.NormalsData.resize(VertexCount * NormalDataSize);
		}
		else
		{
			Info.NormalsData.resize(0);
		};

		switch(DetailsMode)
		{
		case VertexDetailsMode::Lists:
			memcpy(&Info.PositionData[0], &((uint8 *)Data)[PositionOffset], PositionDataSize * VertexCount);

			if(TexCoordDataSize)
			{
				memcpy(&Info.TexCoordData[0], &((uint8 *)Data)[TexCoordOffset], TexCoordDataSize * VertexCount);
			};

			if(ColorDataSize)
			{
				memcpy(&Info.ColorData[0], &((uint8 *)Data)[ColorOffset], ColorDataSize * VertexCount);
			};

			if(NormalDataSize)
			{
				memcpy(&Info.NormalsData[0], &((uint8 *)Data)[NormalOffset], NormalDataSize * VertexCount);
			};

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
					};

					if(ColorDataSize > 0)
					{
						memcpy(&Info.ColorData[ColorByteOffset], &((uint8 *)Data)[i + ColorOffset], ColorDataSize);

						ColorByteOffset += ColorDataSize;
					};

					if(TexCoordDataSize > 0)
					{
						memcpy(&Info.TexCoordData[TexCoordByteOffset], &((uint8 *)Data)[i + TexCoordOffset], TexCoordDataSize);

						TexCoordByteOffset += TexCoordDataSize;
					};

					if(NormalDataSize > 0)
					{
						memcpy(&Info.NormalsData[NormalByteOffset], &((uint8 *)Data)[i + NormalOffset], NormalDataSize);

						NormalByteOffset += NormalDataSize;
					};
				};
			};

			break;
		};

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
			};

			if(TexCoordDataSize)
			{
				Info.TexCoordOffset = Offset;

				Offset += TexCoordDataSize;
			}
			else
			{
				Info.TexCoordOffset = -1;
			};

			if(ColorDataSize)
			{
				Info.ColorOffset = Offset;

				Offset += ColorDataSize;
			}
			else
			{
				Info.ColorOffset = -1;
			};

			if(NormalDataSize)
			{
				Info.NormalsOffset = Offset;

				Offset += NormalDataSize;
			}
			else
			{
				Info.NormalsOffset = -1;
			};

			for(uint32 i = 0, index = 0; i < DataByteSize; i += VertexSize, index++)
			{
				if(Info.PositionOffset != -1)
				{
					memcpy(&Info.CombinedData[i + Info.PositionOffset], &Info.PositionData[index * PositionDataSize], PositionDataSize);
				};

				if(Info.ColorOffset != -1)
				{
					memcpy(&Info.CombinedData[i + Info.ColorOffset], &Info.ColorData[index * ColorDataSize], ColorDataSize);
				};

				if(Info.TexCoordOffset != -1)
				{
					memcpy(&Info.CombinedData[i + Info.TexCoordOffset], &Info.TexCoordData[index * TexCoordDataSize], TexCoordDataSize);
				};

				if(Info.NormalsOffset != -1)
				{
					memcpy(&Info.CombinedData[i + Info.NormalsOffset], &Info.NormalsData[index * NormalDataSize], NormalDataSize);
				};
			};

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
			};

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			GLCHECK();

			LastBoundVBO = 0;
		};
	};

	bool SFMLRendererImplementation::IsVertexBufferHandleValid(VertexBufferHandle Handle)
	{
		return Handle != 0 && VertexBuffers.find(Handle) != VertexBuffers.end();
	};

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
		};

		VertexBuffers.erase(it);
	};

	void SFMLRendererImplementation::RenderVertices(uint32 VertexMode, VertexBufferHandle Buffer, uint32 Start, uint32 End)
	{
		FlushRenderText();

		FrameStatsValue.StateChanges++;

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

		FrameStatsValue.DrawCalls++;
		FrameStatsValue.VertexCount += End - Start;

		if(LastBoundTexture != 0)
		{
			EnableState(GL_TEXTURE_2D);
		}
		else
		{
			DisableState(GL_TEXTURE_2D);
		};

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
				};
			}
			else
			{
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				GLCHECK();
				LastBoundVBO = 0;
			};
		};

		if(SupportsVBOs && glIsBuffer(it->second.VBOID))
		{
			glVertexPointer(VertexBufferDataElementCount[it->second.PositionDataType], GL_FLOAT, it->second.VertexSize, BUFFER_OFFSET(it->second.PositionOffset));
		}
		else
		{
			glVertexPointer(VertexBufferDataElementCount[it->second.PositionDataType], GL_FLOAT, 0, &it->second.PositionData[0]);
		};

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
		};

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
		};

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
		};

		glDrawArrays(VertexBufferVertexMode[VertexMode], Start, End - Start);

		GLCHECK();
	};

	void SFMLRendererImplementation::StartClipping(const Rect &ClippingRect)
	{
		FrameStatsValue.StateChanges++;
		FrameStatsValue.ClippingChanges++;

		EnableState(GL_SCISSOR_TEST);

		GLint x = (GLint)ClippingRect.Left, y = (GLint)(Size().y - ClippingRect.Bottom);

		GLsizei Width = (GLsizei)(ClippingRect.Right - ClippingRect.Left), Height = (GLsizei)(ClippingRect.Bottom - ClippingRect.Top);

		glScissor(x, y, Width, Height);

		GLCHECK();
	};

	void SFMLRendererImplementation::FinishClipping()
	{
		FrameStatsValue.StateChanges++;
		FrameStatsValue.ClippingChanges++;

		DisableState(GL_SCISSOR_TEST);

		GLCHECK();
	};

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
	};

	void SFMLRendererImplementation::Display()
	{
		FrameStatsValue.RendererCustomMessage = "";

		if(ExtensionsAvailable)
		{
			FrameStatsValue.RendererCustomMessage += "SupportsExtensions";
		};

		if(SupportsVBOs)
		{
			FrameStatsValue.RendererCustomMessage += " VBOEnabled";
		};

		FrameStatsValue.RendererCustomMessage += " [Saved " + StringUtils::MakeIntString(SavedTextDrawcalls) + " Text Drawcalls]";

		PreviousFrameStatsValue = FrameStatsValue;
		FrameStatsValue.Clear();

		FlushRenderText();

		FrameStatsValue.TotalResources = Fonts.size() + VertexBuffers.size();
		FrameStatsValue.TotalResourceUsage = 0;

		for(FontMap::iterator it = Fonts.begin(); it != Fonts.end(); it++)
		{
			//We cannot verify all textures so we'll only report the client usage...
			FrameStatsValue.TotalResourceUsage += it->second.ResourceSize / ONE_MB_FLOAT;
		};

		for(VertexBufferMap::iterator it = VertexBuffers.begin(); it != VertexBuffers.end(); it++)
		{
			FrameStatsValue.TotalResourceUsage += (it->second.PositionData.size() + it->second.TexCoordData.size() + it->second.NormalsData.size() + it->second.ColorData.size()) / ONE_MB_FLOAT;
		};

		for(TextureMap::iterator it = Textures.begin(); it != Textures.end(); it++)
		{
			//Multiplied by 8 instead of 4 due to double RAM usage
			FrameStatsValue.TotalResourceUsage += (it->second.Width * it->second.Height * 8) / ONE_MB_FLOAT;
		};

		SavedTextDrawcalls = 0;

		Window.display();
	};

	const RendererFrameStats &SFMLRendererImplementation::FrameStats() const
	{
		return PreviousFrameStatsValue;
	};

	void SFMLRendererImplementation::SetWorldMatrix(const Matrix4x4 &WorldMatrix)
	{
		FrameStatsValue.StateChanges++;
		FrameStatsValue.MatrixChanges++;

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&WorldMatrix.m[0][0]);

		GLCHECK();
	};

	void SFMLRendererImplementation::SetProjectionMatrix(const Matrix4x4 &ProjectionMatrix)
	{
		FrameStatsValue.StateChanges++;
		FrameStatsValue.MatrixChanges++;

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(&ProjectionMatrix.m[0][0]);
		glMatrixMode(GL_MODELVIEW);

		GLCHECK();
	};

	void SFMLRendererImplementation::SetViewport(f32 x, f32 y, f32 Width, f32 Height)
	{
		FrameStatsValue.StateChanges++;

		glViewport((GLint)x, (GLint)y, (GLsizei)Width, (GLsizei)Height);

		GLCHECK();
	};

	TextureHandle SFMLRendererImplementation::CreateTexture()
	{
		FrameStatsValue.StateChanges++;

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
		FrameStatsValue.StateChanges++;

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
		FrameStatsValue.StateChanges++;

		TextureHandle Last = LastBoundTexture;

		BindTexture(Handle);

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
	};

	void SFMLRendererImplementation::SetTextureWrapMode(TextureHandle Handle, uint32 WrapMode)
	{
		FrameStatsValue.StateChanges++;

		TextureHandle Last = LastBoundTexture;

		BindTexture(Handle);
		
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

			GLCHECK();

			break;

		case TextureWrapMode::Clamp:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			GLCHECK();

			break;

		case TextureWrapMode::ClampToBorder:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

			GLCHECK();

			break;

		case TextureWrapMode::ClampToEdge:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			GLCHECK();

			break;
		};

		BindTexture(Last);
	};

	void SFMLRendererImplementation::SetTextureFiltering(TextureHandle Handle, uint32 Filter)
	{
		FrameStatsValue.StateChanges++;

		TextureHandle Last = LastBoundTexture;

		BindTexture(Handle);

		if(!IsTextureHandleValid(Handle))
		{
			if(Last)
				BindTexture(Last);

			return;
		};

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

			glGenerateMipmap(GL_TEXTURE_2D);

			GLCHECK();
		};

		BindTexture(Last);
	};

	bool SFMLRendererImplementation::CaptureScreen(uint8 *Pixels, uint32 BufferByteCount)
	{
		FrameStatsValue.StateChanges++;

		if(BufferByteCount != (uint32)(Window.getSize().x * Window.getSize().y * 4))
			return false;

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
		};

		GLCHECK();

		return true;
	};

	bool SFMLRendererImplementation::GetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 BufferByteCount)
	{
		FrameStatsValue.StateChanges++;

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
			glBlendEquation(GL_FUNC_SUBTRACT);

			break;
		};

		GLCHECK();
	};

	void SFMLRendererImplementation::BindTexture(TextureHandle Handle)
	{
		if(Handle == LastBoundTexture)
			return;

		FrameStatsValue.TextureChanges++;

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
		return (void *)Window.getSystemHandle();
	};

	void SFMLRendererImplementation::SetFrameRate(uint32 FPS)
	{
		Window.setFramerateLimit(FPS);
	};

	FontHandle SFMLRendererImplementation::CreateFont(Stream *Data)
	{
		FrameStatsValue.StateChanges++;

		FontCounter++;

		FontInfo &TheFont = Fonts[FontCounter];

		TheFont.ActualFont.Reset(new sf::Font());
		TheFont.Data.resize((uint32)Data->Length());

		Data->AsBuffer(&TheFont.Data[0], (uint32)Data->Length());

		if(!TheFont.ActualFont->loadFromMemory(&TheFont.Data[0], TheFont.Data.size()))
		{
			Log::Instance.LogErr(TAG, "Failed to create a font from a stream '%s' of size '%d'", StringUtils::PointerString(Data).c_str(), Data->Length());

			Fonts.erase(FontCounter);

			return 0;
		};

		TheFont.ResourceSize = (uint32)Data->Length();

		return FontCounter;
	};

	void SFMLRendererImplementation::DestroyFont(FontHandle Handle)
	{
		FrameStatsValue.StateChanges++;

		FontMap::iterator it = Fonts.find(Handle);

		if(it != Fonts.end())
			Fonts.erase(it);
	};

	Rect SFMLRendererImplementation::MeasureText(FontHandle Handle, const std::string &TheText, const TextParams &Parameters)
	{
		FrameStatsValue.StateChanges++;

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
		//We multiply by two because we not only need to add the width/height to top and bottom, but also
		//the width and height already have that reduced as well, and we're working with LRTB, not LRWH
		return Rect(InRect.left, InRect.width + InRect.left * 2, InRect.top, InRect.height + InRect.top * 2);
	};

	void SFMLRendererImplementation::RenderText(FontHandle Handle, const std::string &TheText, const TextParams &Parameters)
	{
		FrameStatsValue.StateChanges++;
		FrameStatsValue.TextureChanges++;

		if(TheText.length() == 0)
			return;

		std::stringstream UniqueString;

		UniqueString << "BC:" << Parameters.BorderColorValue.ToString() << "BS:" << Parameters.BorderSizeValue << "FS:" << Parameters.FontSizeValue << "FH:" << Parameters.FontValue <<
			"TC:" << Parameters.TextColorValue.ToString() << "STC:" <<Parameters.SecondaryTextColorValue.ToString() << "SV: " << Parameters.StyleValue;

		StringID UniqueStringID = MakeStringID(UniqueString.str());

		if(UniqueStringID != UniqueCacheStringID)
		{
			FlushRenderText();

			UniqueCacheStringID = UniqueStringID;
		};

		Vector2 ActualPosition = Parameters.PositionValue;

		FontMap::iterator FontIterator = Fonts.find(Handle);

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

		TheRenderTextCache.Positions.insert(TheRenderTextCache.Positions.end(), Positions.begin(), Positions.end());
		TheRenderTextCache.Colors.insert(TheRenderTextCache.Colors.end(), Colors.begin(), Colors.end());
		TheRenderTextCache.TexCoords.insert(TheRenderTextCache.TexCoords.end(), TexCoords.begin(), TexCoords.end());
		TheRenderTextCache.TheTexture = const_cast<sf::Texture *>(FontTexture);

#if USE_TEXT_CACHE
		ReportSkippedDrawCall();
		SavedTextDrawcalls++;
#else
		FlushRenderText();
#endif
	};

	void SFMLRendererImplementation::FlushRenderText()
	{
		if(UniqueCacheStringID == 0 || TheRenderTextCache.Positions.size() == 0 || TheRenderTextCache.TheTexture == NULL)
			return;

		FrameStatsValue.DrawCalls++;
		FrameStatsValue.TextureChanges += 2;
		FrameStatsValue.VertexCount += TheRenderTextCache.Positions.size();

		UniqueCacheStringID = 0;

		LastBoundVBO = 0;

		if(SupportsVBOs)
		{
			FrameStatsValue.StateChanges++;

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		};

		TextureHandle PreviousTexture = LastBoundTexture;

		BindTexture((TextureHandle)0);

		sf::Texture::bind(TheRenderTextCache.TheTexture);

		EnableState(GL_VERTEX_ARRAY);
		EnableState(GL_TEXTURE_COORD_ARRAY);
		EnableState(GL_COLOR_ARRAY);
		DisableState(GL_NORMAL_ARRAY);

		bool TextureWasEnabled = IsStateEnabled(GL_TEXTURE_2D);

		EnableState(GL_TEXTURE_2D);

		glVertexPointer(2, GL_FLOAT, 0, &TheRenderTextCache.Positions[0]);
		glTexCoordPointer(2, GL_FLOAT, 0, &TheRenderTextCache.TexCoords[0]);
		glColorPointer(4, GL_FLOAT, 0, &TheRenderTextCache.Colors[0]);

		glDrawArrays(GL_TRIANGLES, 0, TheRenderTextCache.Positions.size());

		if(PreviousTexture != 0)
		{
			BindTexture(PreviousTexture);
		};

		if(!TextureWasEnabled)
		{
			DisableState(GL_TEXTURE_2D);
		};

		TheRenderTextCache.Positions.resize(0);
		TheRenderTextCache.Colors.resize(0);
		TheRenderTextCache.TexCoords.resize(0);
		TheRenderTextCache.TheTexture = NULL;
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

		FrameStatsValue.StateChanges++;

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

		GLCHECK();

		GLStates[ID] = true;
	};

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

	void SFMLRendererImplementation::ReportSkippedDrawCall()
	{
		FrameStatsValue.SkippedDrawCalls++;
	};

#	endif
};
