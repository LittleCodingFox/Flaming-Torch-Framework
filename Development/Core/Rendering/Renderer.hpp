#if USE_GRAPHICS

#	define SCENEPASS_END "END"

/*!
*	Renderer is a rendering class that takes care of windowing and OpenGL Contexts
*/
class Renderer
{
	friend class RendererManager;
	friend class InputCenter;
	friend class Texture;
	friend class GameInterface;
private:
	IRendererImplementation *Impl;
	Vector2 BaseResolutionValue;

	Renderer(IRendererImplementation *_Impl);
	Renderer(const Renderer &o);
	Renderer &operator=(const Renderer &o);
	RendererHandle HandleValue;

	struct MatrixStackElement
	{
		Matrix4x4 World, Projection;
	};

	std::vector<MatrixStackElement> MatrixStack;

	Matrix4x4 LastWorldMatrix, LastProjectionMatrix;

	std::vector<Rect> ClippingStack;
	std::vector<DisposablePointer<ScenePass> > ScenePasses;
public:
	Camera RenderCamera;

	/*!
	*	OnFrameStarted should be used to do stuff before we render the main frame such as clearing the screen
	*	OnFrameEnded should be used to draw stuff after the frame has ended, such as our HUD
	*	OnFrameDraw should be used for drawing the main content of the frame
	*	\note the ScenePass might have a hardcoded name of END, which means it will display its contents on the screen afterwards
	*/
	SimpleDelegate::SimpleDelegate<Renderer *, const std::string &> OnFrameStarted, OnFrameEnded, OnFrameDraw;

	/*!
	*	OnResized should be used to reset your camera and projection matrix
	*	Parameters are Renderer, Width, Height
	*/
	SimpleDelegate::SimpleDelegate<Renderer *, uint32, uint32> OnResized;

	/*!
	*	On Resources Reloaded should be used to reset any states that are not usually saved by the renderer
	*	Might be removed later on
	*/
	SimpleDelegate::SimpleDelegate<Renderer *> OnResourcesReloaded;

	/*!
	*	UI Manager
	*/
	DisposablePointer<UIManager> UI;

	/*!
	*	\return the Renderer Handle
	*/
	RendererHandle Handle() const;

	/*!
	*	Adds a scene pass
	*	\param Name the name of the scene pass to add
	*/
	void AddScenePass(const std::string &Name);

	/*!
	*	Removes a scene pass
	*	\param Name the name of the scene pass to remove
	*/
	void RemoveScenePass(const std::string &Name);

	/*!
	*	Creates a renderer from a Window Handle
	*	\param WindowHandle the window handle
	*	\param ExpectedCaps the expected capabilities of this renderer
	*/
	bool Create(void *WindowHandle, RendererCapabilities ExpectedCaps = RendererCapabilities());

	/*!
	*	Creates a renderer from window details
	*	\param Title the title of the window
	*	\param Width the width of the window
	*	\param Height the height of the window
	*	\param Style the style of the window (one of RendererWindowStyle::*)
	*	\param ExpectedCaps the expected capabilities of this renderer
	*/
	bool Create(const std::string &Title, uint32 Width, uint32 Height, uint32 Style, RendererCapabilities ExpectedCaps = RendererCapabilities());

	/*!
	*	\return the currently available renderer capabilities
	*/
	const RendererCapabilities &Capabilities() const;

	/*!
	*	\return the Desktop Display Mode
	*/
	static RendererDisplayMode DesktopDisplayMode();

	/*!
	*	\return all available display modes
	*/
	static std::vector<RendererDisplayMode> DisplayModes();

	/*!
	*	Render Window Size
	*/
	Vector2 Size() const;

	/*!
	*	\param Character the character to get the glyph for
	*	\param Parameters the text parameters
	*	\return the Text Glyph Info of this glyph
	*/
	TextGlyphInfo GetTextGlyph(uint32 Character, const TextParams &Parameters);

	/*!
	*	\param Prev the previous character
	*	\param Cur the current character
	*	\param Parameters the text parameters
	*	\return the kerning space between characters
	*/
	int32 GetTextKerning(uint32 Prev, uint32 Cur, const TextParams &Parameters);
	
	/*!
	*	Creates a Vertex Buffer
	*	\return a vertex buffer handle, or INVALID_FTGHANDLE on error
	*/
	VertexBufferHandle CreateVertexBuffer();

	/*!
	*	Sets a Vertex Buffer's Data
	*	\param Handle the Vertex Buffer's Handle
	*	\param DetailsMode one of VertexDetailsMode::*
	*	\param Elements a list of VertexElementDescriptor
	*	\param ElementCount the amount of VertexElements in Elements
	*	\param Data the data for the vertex buffer
	*	\param DataByteSize the byte size of the entire data
	*/
	void SetVertexBufferData(VertexBufferHandle Handle, uint8 DetailsMode, VertexElementDescriptor *Elements, uint32 ElementCount, const void *Data, uint32 DataByteSize);

	/*!
	*	Checks whether a vertex buffer handle is valid
	*	\param Handle the Vertex Buffer Handle to check
	*/
	bool IsVertexBufferHandleValid(VertexBufferHandle Handle);

	/*!
	*	Destroys a Vertex Buffer
	*	\param Handle the handle of the vertex buffer to destroy
	*/
	void DestroyVertexBuffer(VertexBufferHandle Handle);

	/*!
	*	Creates a Frame Buffer
	*	\param Info the creation info for the buffer
	*	\return a FrameBufferHandle or INVALID_FTGHANDLE
	*/
	FrameBufferHandle CreateFrameBuffer(const FrameBufferCreationInfo &Info);

	/*!
	*	\param Handle the FrameBufferHandle to bind
	*	\return whether Handle is valid
	*/
	bool IsFrameBufferValid(FrameBufferHandle Handle);

	/*!
	*	Binds a FrameBuffer for rendering
	*	\param Handle the FrameBufferHandle to bind
	*/
	void BindFrameBuffer(FrameBufferHandle Handle);

	/*!
	*	Destroys a Frame Buffer
	*	\param Handle the FrameBufferHandle to destroy
	*/
	void DestroyFrameBuffer(FrameBufferHandle Handle);

	/*!
	*	Render vertices
	*	\param VertexMode one of VertexModes::*
	*	\param Buffer is the handle of the Vertex Buffer
	*	\param Start the starting vertex
	*	\param End the ending vertex
	*/
	void RenderVertices(uint32 VertexMode, VertexBufferHandle Buffer, uint32 Start, uint32 End);

	/*!
	*	Start clipping the rendering to a rectangle
	*	\param ClippingRect the clipping rectangle
	*	\note The orientation is always from bottom left and up, so e.g., to clip a 100x100 rect on the top right of a screen of 1000x1000px size, you must pass a rect of (900, 1000, 1000, 900)
	*	Therefore to keep up with the usual orientation we use (+y is "down" in the window), we emulate that orientation by flipping the top/bottom fields of Rect. So now the previous example becomes (900, 1000, 900, 1000)
	*/
	void StartClipping(const Rect &ClippingRect);

	/*!
	*	Finishes clipping the rendering from a rectangle
	*/
	void FinishClipping();

	/*!
	*	Clear a render buffer
	*	\param Buffers a Bitmask of RenderBuffer::*
	*/
	void Clear(uint32 Buffers);

	/*!
	*	Displays a frame
	*/
	void Display();

	/*!
	*	Gets the last Frame Statistics
	*/
	const RendererFrameStats &FrameStats() const;

	/*!
	*	Sets the current World Matrix
	*	\param WorldMatrix the new World Matrix
	*/
	void SetWorldMatrix(const Matrix4x4 &WorldMatrix);

	/*!
	*	Sets the current Projection Matrix
	*	\param ProjectionMatrix the new World Matrix
	*/
	void SetProjectionMatrix(const Matrix4x4 &ProjectionMatrix);

	/*!
	*	Pushes (stores) the currently used world and projection matrices
	*/
	void PushMatrices();

	/*!
	*	Pops (restores) the last used world and projection matrices
	*/
	void PopMatrices();

	/*!
	*	Gets the current World Matrix
	*/
	const Matrix4x4 &WorldMatrix();

	/*!
	*	Gets the current projection matrix
	*/
	const Matrix4x4 &ProjectionMatrix();

	/*!
	*	Sets the current viewport
	*	\param x the viewport's x position
	*	\param y the viewport's y position
	*	\param Width the viewport's width
	*	\param Height the viewport's height
	*/
	void SetViewport(f32 x, f32 y, f32 Width, f32 Height);

	/*!
	*	Creates a texture
	*	\return a texture handle, or 0 on error
	*/
	TextureHandle CreateTexture();

	/*!
	*	Check whether a texture handle is still valid
	*	\param Handle the texture's handle
	*/
	bool IsTextureHandleValid(TextureHandle Handle);

	/*!
	*	Destroys a texture
	*	\param Handle the texture handle to destroy
	*/
	void DestroyTexture(TextureHandle Handle);

	/*!
	*	Fills in a texture's data
	*	\param Handle the texture's handle
	*	\param Pixels an array of RGBA pixels
	*	\param Width the width of the texture
	*	\param Height the height of the texture
	*/
	void SetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 Width, uint32 Height);

	/*!
	*	Gets a texture's data to a pixel buffer
	*	\param Handle the texture's handle
	*	\param Pixels the pixels to store the screen to
	*	\param BufferByteCount the byte count of the buffer. It should always be equal to Width x Height x 4 bytes
	*	\return whether we were able to capture the screen
	*/
	bool GetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 BufferByteCount);

	/*!
	*	Sets a texture's wrap mode
	*	\param Handle the texture's Handle
	*	\param WrapMode one of TextureWrapMode::*
	*/
	void SetTextureWrapMode(TextureHandle Handle, uint32 WrapMode);

	/*!
	*	Sets a texture's filter mode
	*	\param Handle the texture's Handle
	*	\param Filtering one of TextureFiltering::*
	*/
	void SetTextureFiltering(TextureHandle Handle, uint32 Filtering);

	/*!
	*	Sets the current blending mode
	*	\param BlendingMode one of BlendingMode::*
	*/
	void SetBlendingMode(uint32 BlendingMode);

	/*!
	*	Binds a texture for use
	*	\param Handle the texture's handle
	*/
	void BindTexture(TextureHandle Handle);

	/*!
	*	Binds a texture for use
	*	\param TheTexture the texture to bind
	*/
	void BindTexture(Texture *TheTexture);

	/*!
	*	Captures the screen to a pixel buffer
	*	\param Pixels the pixels to store the screen to
	*	\param BufferByteCount the byte count of the buffer. It should always be equal to Width x Height x 4 bytes
	*	\return whether we were able to capture the screen
	*/
	bool CaptureScreen(uint8 *Pixels, uint32 BufferByteCount);

	/*!
	*	Polls for new renderer events
	*	\param Out the RendererEvent we received
	*	\return true if there's a new event, false otherwise
	*/
	bool PollEvent(RendererEvent &Out);

	/*!
	*	Creates a Font from a Stream of Data
	*	\param Data the data of the font
	*	\return a FontHandle or 0xFFFFFFFF
	*/
	FontHandle CreateFont(Stream *Data);

	/*!
	*	Destroys a font
	*	\param Handle the font's handle
	*/
	void DestroyFont(FontHandle Handle);

	/*!
	*	Measures Text
	*	\param Text the text to measure
	*	\param Parameters the text parameters to use
	*	\return a rectangle with the text's bounds
	*/
	Rect MeasureText(const std::string &Text, const TextParams &Parameters);

	/*!
	*	Renders Text to the screen
	*	\param Text the text to measure
	*	\param Parameters the text parameters to use
	*/
	void RenderText(const std::string &Text, const TextParams &Parameters);

	/*!
	*	Gets the Native Window Handle of this Renderer
	*/
	void *WindowHandle() const;

	/*!
	*	Sets the current Mouse Position
	*	\param Position the new Mouse Position
	*/
	void SetMousePosition(const Vector2 &Position);

	/*!
	*	Sets the Rendering Frame Rate
	*	\param FPS the target FPS
	*/
	void SetFrameRate(uint32 FPS);

	/*!
	*	Reports a Skipped Draw Call (used to help figure out how much we're optimizing by skipping unnecessary drawcalls)
	*/
	void ReportSkippedDrawCall();

	/*!
	*	\return the amount of matrices on our Matrix Stack 
	*	\sa PopMatrix
	*	\sa PushMatrix
	*	\sa SetWorldMatrix
	*	\sa SetProjectionMatrix
	*/
	uint32 MatrixStackSize() const;

	/*!
	*	\return the base resolution of the render
	*/
	const Vector2 &BaseResolution() const;

	/*!
	*	Scales a coordinate by the base resolution
	*	\param Coordinate the coordinate to scale by the base resolution
	*/
	Vector2 ScaleCoordinate(const Vector2 &Coordinate) const;
};
#endif
