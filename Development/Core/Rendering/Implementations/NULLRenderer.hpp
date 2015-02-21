class NULLRendererImplementation : public IRendererImplementation
{
public:
	/*!
	*	Creates a renderer from a Window Handle
	*	\param WindowHandle the window handle
	*	\param ExpectedCaps the expected capabilities of this renderer
	*/
	bool Create(void *WindowHandle, RendererCapabilities ExpectedCaps) override;

	/*!
	*	Creates a renderer from window details
	*	\param Title the title of the window
	*	\param Width the width of the window
	*	\param Height the height of the window
	*	\param Style the style of the window (one of RendererWindowStyle::*)
	*	\param ExpectedCaps the expected capabilities of this renderer
	*/
	bool Create(const std::string &Title, uint32 Width, uint32 Height, uint32 Style, RendererCapabilities ExpectedCaps) override;

	/*!
	*	\return the currently available renderer capabilities
	*/
	const RendererCapabilities &Capabilities() const override;

	/*!
	*	\return the Desktop Display Mode
	*/
	RendererDisplayMode DesktopDisplayMode() override;

	/*!
	*	\return all available display modes
	*/
	std::vector<RendererDisplayMode> DisplayModes() override;

	/*!
	*	Render Window Size
	*/
	Vector2 Size() const override;

	/*!
	*	Creates a Vertex Buffer
	*	\return a vertex buffer handle, or 0 on error
	*/
	VertexBufferHandle CreateVertexBuffer() override;

	/*!
	*	Checks whether a vertex buffer handle is valid
	*	\param Handle the Vertex Buffer Handle to check
	*/
	bool IsVertexBufferHandleValid(VertexBufferHandle Handle) override;

	/*!
	*	Sets a Vertex Buffer's Data
	*	\param Handle the Vertex Buffer's Handle
	*	\param DetailsMode one of VertexDetailsMode::*
	*	\param Elements a list of VertexElementDescriptor
	*	\param ElementCount the amount of VertexElements in Elements
	*	\param Data the data for the vertex buffer
	*	\param DataByteSize the byte size of the entire data
	*/
	void SetVertexBufferData(VertexBufferHandle Handle, uint8 DetailsMode, VertexElementDescriptor *Elements, uint32 ElementCount, const void *Data, uint32 DataByteSize) override;

	/*!
	*	Destroys a Vertex Buffer
	*	\param Handle the handle of the vertex buffer to destroy
	*/
	void DestroyVertexBuffer(VertexBufferHandle Handle) override;

	/*!
	*	Render vertices
	*	\param VertexMode one of VertexModes::*
	*	\param Buffer is the handle of the Vertex Buffer
	*	\param Start the starting vertex
	*	\param End the ending vertex
	*/
	void RenderVertices(uint32 VertexMode, VertexBufferHandle Buffer, uint32 Start, uint32 End) override;

	/*!
	*	Clips the rendering to a rectangle
	*	\param ClippingRect the clipping rectangle
	*	\note The orientation is always from bottom left and up, so e.g., to clip a 100x100 rect on the top right of a screen of 1000x1000px size, you must pass a rect of (900, 1000, 1000, 900)
	*	Therefore to keep up with the usual orientation we use (+y is "down" in the window), we emulate that orientation by flipping the top/bottom fields of Rect. So now the previous example becomes (900, 1000, 900, 1000)
	*/
	void SetClipRect(const Rect &ClippingRect) override;

	/*!
	*	Clear a render buffer
	*	\param Buffers a Bitmask of RenderBuffer::*
	*/
	void Clear(uint32 Buffers) override;

	/*!
	*	Displays a frame
	*/
	void Display() override;

	/*!
	*	Gets the last Frame Statistics
	*/
	const RendererFrameStats &FrameStats() const override;

	/*!
	*	Sets the current World Matrix
	*	\param WorldMatrix the new World Matrix
	*/
	void SetWorldMatrix(const Matrix4x4 &WorldMatrix) override;

	/*!
	*	Sets the current Projection Matrix
	*	\param ProjectionMatrix the new World Matrix
	*/
	void SetProjectionMatrix(const Matrix4x4 &ProjectionMatrix) override;

	/*!
	*	Sets the current viewport
	*	\param x the viewport's x position
	*	\param y the viewport's y position
	*	\param Width the viewport's width
	*	\param Height the viewport's height
	*/
	void SetViewport(f32 x, f32 y, f32 Width, f32 Height) override;

	/*!
	*	Creates a texture
	*	\return a texture handle, or 0 on error
	*/
	TextureHandle CreateTexture() override;

	/*!
	*	Check whether a texture handle is still valid
	*	\param Handle the texture's handle
	*/
	bool IsTextureHandleValid(TextureHandle Handle) override;

	/*!
	*	Destroys a texture
	*	\param Handle the texture handle to destroy
	*/
	void DestroyTexture(TextureHandle Handle) override;

	/*!
	*	Fills in a texture's data
	*	\param Handle the texture's handle
	*	\param Pixels an array of RGBA pixels
	*	\param Width the width of the texture
	*	\param Height the height of the texture
	*/
	void SetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 Width, uint32 Height) override;

	/*!
	*	Gets a texture's data to a pixel buffer
	*	\param Handle the texture's handle
	*	\param Pixels the pixels to store the screen to
	*	\param BufferByteCount the byte count of the buffer. It should always be equal to Width x Height x 4 bytes
	*	\return whether we were able to capture the screen
	*/
	bool GetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 BufferByteCount) override;

	/*!
	*	Sets a texture's wrap mode
	*	\param Handle the texture's Handle
	*	\param WrapMode one of TextureWrapMode::*
	*/
	void SetTextureWrapMode(TextureHandle Handle, uint32 WrapMode) override;

	/*!
	*	Sets a texture's filter mode
	*	\param Handle the texture's Handle
	*	\param Filtering one of TextureFiltering::*
	*/
	void SetTextureFiltering(TextureHandle Handle, uint32 Filtering) override;

	/*!
	*	Sets the current blending mode
	*	\param BlendingMode one of BlendingMode::*
	*/
	void SetBlendingMode(uint32 BlendingMode) override;

	/*!
	*	Binds a texture for use
	*	\param Handle the texture's handle
	*/
	void BindTexture(TextureHandle Handle) override;

	/*!
	*	Captures the screen to a pixel buffer
	*	\param Pixels the pixels to store the screen to
	*	\param BufferByteCount the byte count of the buffer. It should always be equal to Width x Height x 4 bytes
	*	\return whether we were able to capture the screen
	*/
	bool CaptureScreen(uint8 *Pixels, uint32 BufferByteCount) override;

	/*!
	*	Polls for new renderer events
	*	\param Out the RendererEvent we received
	*	\return true if there's a new event, false otherwise
	*/
	bool PollEvent(RendererEvent &Out) override;

	/*!
	*	Gets the Native Window Handle of this Renderer
	*/
	void *WindowHandle() const override;

	/*!
	*	Sets the current Mouse Position
	*	\param Position the new Mouse Position
	*/
	void SetMousePosition(const Vector2 &Position) override;

	/*!
	*	Sets the Rendering Frame Rate
	*	\param FPS the target FPS
	*/
	void SetFrameRate(uint32 FPS) override;

	/*!
	*	Reports a Skipped Draw Call (used to help figure out how much we're optimizing by skipping unnecessary drawcalls)
	*/
	void ReportSkippedDrawCall() override;
};
