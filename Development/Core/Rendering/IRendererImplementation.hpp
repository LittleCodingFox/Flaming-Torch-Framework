#if USE_GRAPHICS
/*!
*	Renderer Implementation interface
*/
class IRendererImplementation
{
public:
	Renderer *Target;

	virtual ~IRendererImplementation() {}

	/*!
	*	Creates a renderer from a Window Handle
	*	\param WindowHandle the window handle
	*	\param ExpectedCaps the expected capabilities of this renderer
	*/
	virtual bool Create(void *WindowHandle, RendererCapabilities ExpectedCaps) = 0;

	/*!
	*	Creates a renderer from window details
	*	\param Title the title of the window
	*	\param Width the width of the window
	*	\param Height the height of the window
	*	\param Style the style of the window (one of RendererWindowStyle::*)
	*	\param ExpectedCaps the expected capabilities of this renderer
	*/
	virtual bool Create(const std::string &Title, uint32 Width, uint32 Height, uint32 Style, RendererCapabilities ExpectedCaps) = 0;

	/*!
	*	\return the currently available renderer capabilities
	*/
	virtual const RendererCapabilities &Capabilities() const = 0;

	/*!
	*	\return the Desktop Display Mode
	*/
	virtual RendererDisplayMode DesktopDisplayMode() = 0;

	/*!
	*	\return all available display modes
	*/
	virtual std::vector<RendererDisplayMode> DisplayModes() = 0;

	/*!
	*	Render Window Size
	*/
	virtual Vector2 Size() const = 0;

	/*!
	*	Creates a Vertex Buffer
	*	\return a vertex buffer handle, or 0 on error
	*/
	virtual VertexBufferHandle CreateVertexBuffer() = 0;

	/*!
	*	Sets a Vertex Buffer's Data
	*	\param Handle the Vertex Buffer's Handle
	*	\param DetailsMode one of VertexDetailsMode::*
	*	\param Elements a list of VertexElementDescriptor
	*	\param ElementCount the amount of VertexElements in Elements
	*	\param Data the data for the vertex buffer
	*	\param DataByteSize the byte size of the entire data
	*/
	virtual void SetVertexBufferData(VertexBufferHandle Handle, uint8 DetailsMode, VertexElementDescriptor *Elements, uint32 ElementCount, const void *Data, uint32 DataByteSize) = 0;

	/*!
	*	Checks whether a vertex buffer handle is valid
	*	\param Handle the Vertex Buffer Handle to check
	*/
	virtual bool IsVertexBufferHandleValid(VertexBufferHandle Handle) = 0;

	/*!
	*	Destroys a Vertex Buffer
	*	\param Handle the handle of the vertex buffer to destroy
	*/
	virtual void DestroyVertexBuffer(VertexBufferHandle Handle) = 0;

	/*!
	*	Creates a Frame Buffer
	*	\param Info the creation info for the buffer
	*	\return a FrameBufferHandle or INVALID_FTGHANDLE
	*/
	virtual FrameBufferHandle CreateFrameBuffer(const FrameBufferCreationInfo &Info) = 0;

	/*!
	*	\param Handle the FrameBufferHandle to bind
	*	\return whether Handle is valid
	*/
	virtual bool IsFrameBufferValid(FrameBufferHandle Handle) = 0;

	/*!
	*	Binds a FrameBuffer for rendering
	*	\param Handle the FrameBufferHandle to bind
	*/
	virtual void BindFrameBuffer(FrameBufferHandle Handle) = 0;

	/*!
	*	Destroys a Frame Buffer
	*	\param Handle the FrameBufferHandle to destroy
	*/
	virtual void DestroyFrameBuffer(FrameBufferHandle Handle) = 0;

	/*!
	*	Render vertices
	*	\param VertexMode one of VertexModes::*
	*	\param Buffer is the handle of the Vertex Buffer
	*	\param Start the starting vertex
	*	\param End the ending vertex
	*/
	virtual void RenderVertices(uint32 VertexMode, VertexBufferHandle Buffer, uint32 Start, uint32 End) = 0;

	/*!
	*	Start clipping the rendering to a rectangle
	*	\param ClippingRect the clipping rectangle
	*	\note The orientation is always from bottom left and up, so e.g., to clip a 100x100 rect on the top right of a screen of 1000x1000px size, you must pass a rect of (900, 1000, 1000, 900)
	*	Therefore to keep up with the usual orientation we use (+y is "down" in the window), we emulate that orientation by flipping the top/bottom fields of Rect. So now the previous example becomes (900, 1000, 900, 1000)
	*/
	virtual void StartClipping(const Rect &ClippingRect) = 0;

	/*!
	*	Finishes clipping the rendering from a rectangle
	*/
	virtual void FinishClipping() = 0;

	/*!
	*	Clear a render buffer
	*	\param Buffers a Bitmask of RenderBuffer::*
	*/
	virtual void Clear(uint32 Buffers) = 0;

	/*!
	*	Displays a frame
	*/
	virtual void Display() = 0;

	/*!
	*	Gets the last Frame Statistics
	*/
	virtual const RendererFrameStats &FrameStats() const = 0;

	/*!
	*	Sets the current World Matrix
	*	\param WorldMatrix the new World Matrix
	*/
	virtual void SetWorldMatrix(const Matrix4x4 &WorldMatrix) = 0;

	/*!
	*	Sets the current Projection Matrix
	*	\param ProjectionMatrix the new World Matrix
	*/
	virtual void SetProjectionMatrix(const Matrix4x4 &ProjectionMatrix) = 0;

	/*!
	*	Sets the current viewport
	*	\param x the viewport's x position
	*	\param y the viewport's y position
	*	\param Width the viewport's width
	*	\param Height the viewport's height
	*/
	virtual void SetViewport(f32 x, f32 y, f32 Width, f32 Height) = 0;

	/*!
	*	Creates a texture
	*	\return a texture handle, or 0 on error
	*/
	virtual TextureHandle CreateTexture() = 0;

	/*!
	*	Check whether a texture handle is still valid
	*	\param Handle the texture's handle
	*/
	virtual bool IsTextureHandleValid(TextureHandle Handle) = 0;

	/*!
	*	Destroys a texture
	*	\param Handle the texture handle to destroy
	*/
	virtual void DestroyTexture(TextureHandle Handle) = 0;

	/*!
	*	Fills in a texture's data
	*	\param Handle the texture's handle
	*	\param Pixels an array of RGBA pixels
	*	\param Width the width of the texture
	*	\param Height the height of the texture
	*/
	virtual void SetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 Width, uint32 Height) = 0;

	/*!
	*	Gets a texture's data to a pixel buffer
	*	\param Handle the texture's handle
	*	\param Pixels the pixels to store the screen to
	*	\param BufferByteCount the byte count of the buffer. It should always be equal to Width x Height x 4 bytes
	*	\return whether we were able to capture the screen
	*/
	virtual bool GetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 BufferByteCount) = 0;

	/*!
	*	Sets a texture's wrap mode
	*	\param Handle the texture's Handle
	*	\param WrapMode one of TextureWrapMode::*
	*/
	virtual void SetTextureWrapMode(TextureHandle Handle, uint32 WrapMode) = 0;

	/*!
	*	Sets a texture's filter mode
	*	\param Handle the texture's Handle
	*	\param Filtering one of TextureFiltering::*
	*/
	virtual void SetTextureFiltering(TextureHandle Handle, uint32 Filtering) = 0;

	/*!
	*	Sets the current blending mode
	*	\param BlendingMode one of BlendingMode::*
	*/
	virtual void SetBlendingMode(uint32 BlendingMode) = 0;

	/*!
	*	Binds a texture for use
	*	\param Handle the texture's handle
	*/
	virtual void BindTexture(TextureHandle Handle) = 0;

	/*!
	*	Captures the screen to a pixel buffer
	*	\param Pixels the pixels to store the screen to
	*	\param BufferByteCount the byte count of the buffer. It should always be equal to Width x Height x 4 bytes
	*	\return whether we were able to capture the screen
	*/
	virtual bool CaptureScreen(uint8 *Pixels, uint32 BufferByteCount) = 0;

	/*!
	*	Polls for new renderer events
	*	\param Out the RendererEvent we received
	*	\return true if there's a new event, false otherwise
	*/
	virtual bool PollEvent(RendererEvent &Out) = 0;

	/*!
	*	Gets the Native Window Handle of this Renderer
	*/
	virtual void *WindowHandle() const = 0;

	/*!
	*	Sets the current Mouse Position
	*	\param Position the new Mouse Position
	*/
	virtual void SetMousePosition(const Vector2 &Position) = 0;

	/*!
	*	Sets the Rendering Frame Rate
	*	\param FPS the target FPS
	*/
	virtual void SetFrameRate(uint32 FPS) = 0;

	/*!
	*	Reports a Skipped Draw Call (used to help figure out how much we're optimizing by skipping unnecessary drawcalls)
	*/
	virtual void ReportSkippedDrawCall() = 0;
};
#endif
