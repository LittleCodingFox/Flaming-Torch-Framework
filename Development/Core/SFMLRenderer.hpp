#pragma once
#if USE_GRAPHICS && USE_SFML_RENDERER
}
#if !__ANDROID__
#	include <GL/glew.h>
#endif
#
#	include <SFML/Graphics.hpp>

namespace FlamingTorch
{
	class SFMLRendererImplementation : public IRendererImplementation
	{
	private:
		static bool FirstRenderer;

		static uint64 TextureCounter, FontCounter, VertexBufferCounter;
	public:
		RendererFrameStats FrameStatsValue, PreviousFrameStatsValue;

		class FontInfo
		{
		public:
			DisposablePointer<sf::Font> ActualFont;
			std::vector<uint8> Data;

			uint32 ResourceSize;

			FontInfo() : ResourceSize(0) {};
			~FontInfo() { ActualFont.Dispose(); };
		};

		class TextureInfo
		{
		public:
			uint32 GLID;
			uint32 Width, Height;
		};

		class VertexBufferInfo
		{
		public:
			std::vector<uint8> PositionData, TexCoordData, NormalsData, ColorData, CombinedData;
			uint8 PositionDataType, TexCoordDataType, NormalsDataType, ColorDataType;
			int32 PositionOffset, TexCoordOffset, NormalsOffset, ColorOffset;

			uint32 VertexSize;
			uint32 VertexCount;
			uint32 VBOID;
		};

		class RenderTextCache
		{
		public:
			const sf::Texture *TheTexture;
			std::vector<Vector2> Positions, TexCoords;
			std::vector<Vector4> Colors;

			RenderTextCache() : TheTexture(NULL) {};
		};

		RenderTextCache TheRenderTextCache;

		sf::RenderWindow Window;
		TextureHandle LastBoundTexture;
		bool SupportsVBOs, ExtensionsAvailable;
		uint32 LastBoundVBO;

		StringID UniqueCacheStringID;

		uint32 SavedTextDrawcalls;

		typedef std::map<FontHandle, FontInfo> FontMap;
		FontMap Fonts;

		typedef std::map<TextureHandle, TextureInfo> TextureMap;
		TextureMap Textures;

		typedef std::map<VertexBufferHandle, VertexBufferInfo> VertexBufferMap;
		VertexBufferMap VertexBuffers;

		typedef std::map<uint32, bool> GLStatesMap;
		GLStatesMap GLStates;

		RendererCapabilities RenderCaps;

		bool BorderlessWindowMode;
		Vector2 OriginalRequestedSize;

		Vector2 LastWindowSize;

		Matrix4x4 LastWorldMatrix, LastProjectionMatrix;

		SFMLRendererImplementation();
		~SFMLRendererImplementation();

		/*
		*	PRIVATE METHODS	
		*/

		void FlushRenderText();

		/*
		*	END PRIVATE METHODS
		*/

		/*!
		*	Creates a renderer from a Window Handle
		*	\param WindowHandle the window handle
		*	\param ExpectedCaps the expected capabilities of this renderer
		*/
		virtual bool Create(void *WindowHandle, RendererCapabilities ExpectedCaps) override;

		/*!
		*	Creates a renderer from window details
		*	\param Title the title of the window
		*	\param Width the width of the window
		*	\param Height the height of the window
		*	\param Style the style of the window (one of RendererWindowStyle::*)
		*	\param ExpectedCaps the expected capabilities of this renderer
		*/
		virtual bool Create(const std::string &Title, uint32 Width, uint32 Height, uint32 Style, RendererCapabilities ExpectedCaps) override;

		/*!
		*	\return the currently available renderer capabilities
		*/
		virtual const RendererCapabilities &Capabilities() const override;

		/*!
		*	\return the Desktop Display Mode
		*/
		virtual RendererDisplayMode DesktopDisplayMode() override;

		/*!
		*	\return all available display modes
		*/
		virtual std::vector<RendererDisplayMode> DisplayModes() override;

		/*!
		*	Render Window Size
		*/
		virtual Vector2 Size() const override;

		/*!
		*	\param Character the character to get the glyph for
		*	\param Parameters the text parameters
		*	\return the Text Glyph Info of this glyph
		*/
		virtual TextGlyphInfo GetTextGlyph(uint32 Character, const TextParams &Parameters) override;

		/*!
		*	\param Prev the previous character
		*	\param Cur the current character
		*	\param Parameters the text parameters
		*	\return the kerning space between characters
		*/
		virtual uint32 GetTextKerning(uint32 Prev, uint32 Cur, const TextParams &Parameters) override;

		/*!
		*	Creates a Vertex Buffer
		*	\return a vertex buffer handle, or 0 on error
		*/
		virtual VertexBufferHandle CreateVertexBuffer() override;

		/*!
		*	Sets a Vertex Buffer's Data
		*	\param Handle the Vertex Buffer's Handle
		*	\param DetailsMode one of VertexDetailsMode::*
		*	\param Elements a list of VertexElementDescriptor
		*	\param ElementCount the amount of VertexElements in Elements
		*	\param Data the data for the vertex buffer
		*	\param DataByteSize the byte size of the entire data
		*/
		virtual void SetVertexBufferData(VertexBufferHandle Handle, uint8 DetailsMode, VertexElementDescriptor *Elements, uint32 ElementCount, const void *Data, uint32 DataByteSize) override;

		/*!
		*	Checks whether a vertex buffer handle is valid
		*	\param Handle the Vertex Buffer Handle to check
		*/
		virtual bool IsVertexBufferHandleValid(VertexBufferHandle Handle) override;

		/*!
		*	Destroys a Vertex Buffer
		*	\param Handle the handle of the vertex buffer to destroy
		*/
		virtual void DestroyVertexBuffer(VertexBufferHandle Handle) override;

		/*!
		*	Render vertices
		*	\param VertexMode one of VertexModes::*
		*	\param Buffer is the handle of the Vertex Buffer
		*	\param Start the starting vertex
		*	\param End the ending vertex
		*/
		virtual void RenderVertices(uint32 VertexMode, VertexBufferHandle Buffer, uint32 Start, uint32 End) override;

		/*!
		*	Start clipping the rendering to a rectangle
		*	\param ClippingRect the clipping rectangle
		*	\note The orientation is always from bottom left and up, so e.g., to clip a 100x100 rect on the top right of a screen of 1000x1000px size, you must pass a rect of (900, 100, 900, 100)
		*/
		virtual void StartClipping(const Rect &ClippingRect) override;

		/*!
		*	Finishes clipping the rendering from a rectangle
		*/
		virtual void FinishClipping() override;

		/*!
		*	Clear a render buffer
		*	\param Buffers a Bitmask of RenderBuffer::*
		*/
		virtual void Clear(uint32 Buffers) override;

		/*!
		*	Displays a frame
		*/
		virtual void Display() override;

		/*!
		*	Gets the last Frame Statistics
		*/
		virtual const RendererFrameStats &FrameStats() const override;

		/*!
		*	Sets the current World Matrix
		*	\param WorldMatrix the new World Matrix
		*/
		virtual void SetWorldMatrix(const Matrix4x4 &WorldMatrix) override;

		/*!
		*	Sets the current Projection Matrix
		*	\param ProjectionMatrix the new World Matrix
		*/
		virtual void SetProjectionMatrix(const Matrix4x4 &ProjectionMatrix) override;

		/*!
		*	Sets the current viewport
		*	\param x the viewport's x position
		*	\param y the viewport's y position
		*	\param Width the viewport's width
		*	\param Height the viewport's height
		*/
		virtual void SetViewport(f32 x, f32 y, f32 Width, f32 Height) override;

		/*!
		*	Creates a texture
		*	\return a texture handle, or 0xFFFFFFFF on error
		*/
		virtual TextureHandle CreateTexture() override;

		/*!
		*	Check whether a texture handle is still valid
		*	\param Handle the texture's handle
		*/
		virtual bool IsTextureHandleValid(TextureHandle Handle) override;

		/*!
		*	Destroys a texture
		*	\param Handle the texture handle to destroy
		*/
		virtual void DestroyTexture(TextureHandle Handle) override;

		/*!
		*	Fills in a texture's data
		*	\param Handle the texture's handle
		*	\param Pixels an array of RGBA pixels
		*	\param Width the width of the texture
		*	\param Height the height of the texture
		*/
		virtual void SetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 Width, uint32 Height) override;

		/*!
		*	Gets a texture's data to a pixel buffer
		*	\param Handle the texture's handle
		*	\param Pixels the pixels to store the screen to
		*	\param BufferByteCount the byte count of the buffer. It should always be equal to Width x Height x 4 bytes
		*	\return whether we were able to capture the screen
		*/
		virtual bool GetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 BufferByteCount) override;

		/*!
		*	Sets a texture's wrap mode
		*	\param Handle the texture's Handle
		*	\param WrapMode one of TextureWrapMode::*
		*/
		virtual void SetTextureWrapMode(TextureHandle Handle, uint32 WrapMode) override;

		/*!
		*	Sets a texture's filter mode
		*	\param Handle the texture's Handle
		*	\param Filtering one of TextureFiltering::*
		*/
		virtual void SetTextureFiltering(TextureHandle Handle, uint32 Filtering) override;

		/*!
		*	Sets the current blending mode
		*	\param BlendingMode one of BlendingMode::*
		*/
		virtual void SetBlendingMode(uint32 BlendingMode) override;

		/*!
		*	Captures the screen to a pixel buffer
		*	\param Pixels the pixels to store the screen to
		*	\param BufferByteCount the byte count of the buffer. It should always be equal to Width x Height x 4 bytes
		*	\return whether we were able to capture the screen
		*/
		virtual bool CaptureScreen(uint8 *Pixels, uint32 BufferByteCount) override;

		/*!
		*	Binds a texture for use
		*	\param Handle the texture's handle
		*/
		virtual void BindTexture(TextureHandle Handle) override;

		/*!
		*	Polls for new renderer events
		*	\param Out the RendererEvent we received
		*	\return true if there's a new event, false otherwise
		*/
		virtual bool PollEvent(RendererEvent &Out) override;

		/*!
		*	Creates a Font from a Stream of Data
		*	\param Data the data of the font
		*	\return a FontHandle or 0xFFFFFFFF
		*/
		virtual FontHandle CreateFont(Stream *Data) override;

		/*!
		*	Destroys a font
		*	\param Handle the font's handle
		*/
		virtual void DestroyFont(FontHandle Handle) override;

		/*!
		*	Measures Text
		*	\param Text the text to measure
		*	\param Parameters the text parameters to use
		*	\return a rectangle with the text's bounds
		*/
		virtual Rect MeasureText(const std::string &Text, const TextParams &Parameters) override;

		/*!
		*	Renders Text to the screen
		*	\param Text the text to measure
		*	\param Parameters the text parameters to use
		*/
		virtual void RenderText(const std::string &Text, const TextParams &Parameters) override;

		/*!
		*	Sets the current Mouse Position
		*	\param Position the new Mouse Position
		*/
		virtual void SetMousePosition(const Vector2 &Position) override;

		/*!
		*	Gets the Native Window Handle of this Renderer
		*/
		virtual void *WindowHandle() const override;

		/*!
		*	Sets the Rendering Frame Rate
		*	\param FPS the target FPS
		*/
		virtual void SetFrameRate(uint32 FPS) override;

		/*!
		*	Enables a GL State
		*	\param ID one GL state enum
		*/
		void EnableState(uint32 ID);

		/*!
		*	Disables a GL State
		*	\param ID one GL state enum
		*/
		void DisableState(uint32 ID);

		/*!
		*	Checks a GL State
		*	\param ID one GL state enum
		*	\return Whether the state is enabled
		*	\note this uses the state cache so if you change states in custom OpenGL code it will be inaccurate
		*/
		bool IsStateEnabled(uint32 ID) const;

		/*!
		*	Reports a Skipped Draw Call (used to help figure out how much we're optimizing by skipping unnecessary drawcalls)
		*/
		virtual void ReportSkippedDrawCall() override;
	};
#endif
