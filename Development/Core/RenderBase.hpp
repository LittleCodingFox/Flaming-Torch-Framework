#pragma once
#
#ifdef CreateFont
#	undef CreateFont
#endif

class InputCenter;
class Texture;
class UIManager;

#define JOYSTICKDEADZONE 30

/*!
*	Vertex Element Type
*/
namespace VertexElementType
{
	enum VertexElementType
	{
		Position, //!<Position of the vertex
		TexCoord, //!<Texture Coordinate of the vertex
		Color, //!<Color of the vertex
		Normal //!<Normal of the vertex
	};
};

/*!
*	Vertex Element Data Type
*/
namespace VertexElementDataType
{
	enum VertexElementDataType
	{
		Float, //!<Single Float
		Float2, //!<Two Floats (Vector2)
		Float3, //!<Three Floats (Vector3)
		Float4, //!<Four Floats (Vector4)
		Count //!<Total data types (do not use)
	};
};

/*!
*	Vertex Details Modes
*/
namespace VertexDetailsMode
{
	enum VertexDetailsMode
	{
		Mixed = 0, //!<Vertices are mixed (e.g., vertices are in format (pos, tex, normal, pos, tex, normal)
		Lists //!<Vertices are lists (e.g., vertices are in format (pos, pos, pos, tex, tex, tex, norma, normal, normal)
	};
};

/*!
*	Vertex Element Descriptor
*/
struct VertexElementDescriptor
{
	uint32 Offset; //!<Byte Offset
	uint8 Type; //!<One of VertexElementType::*
	uint8 DataType; //!<One of VertexElementDataType::*
};

/*!
*	Renderer Vertex Modes namespace
*/
namespace VertexModes
{
	enum VertexModes
	{
		Triangles, //!<Triangle list
		Lines, //!<Line list
		Points, //!<Point list
		Count //!<Total Vertex Modes (do not use)
	};
};

/*!
*	Renderer Window Styles
*/
namespace RendererWindowStyle
{
	enum RendererWindowStyle
	{
		Popup = 0, //!<Window is a popup with no border or title bar
		FullScreen, //!<Window is a fullscreen window
		Default //!<Default Window with a border and title bar
	};
};

/*!
*	Renderer Buffers namespace
*/
namespace RendererBuffer
{
	enum RendererBuffer
	{
		Color = FLAGVALUE(0), //!<Color Buffer
		Depth = FLAGVALUE(1), //!<Depth Buffer
		Stencil = FLAGVALUE(2) //!<Stencil Buffer
	};
};

/*!
*	Renderer Event Type
*/
namespace RendererEventType
{
	enum RendererEventType
	{
		JoystickConnected, //!<Joystick Connected
		JoystickDisconnected, //!Joystick Disconnected
		JoystickButtonPressed, //!<Joystick Button Pressed
		JoystickButtonReleased, //!<Joystick Button Released
		JoystickAxisMoved, //!<Joystick Axis Moved
		MouseMoved, //!<Mouse Moved
		MouseButtonPressed, //!<Mouse Button Pressed
		MouseButtonReleased, //!<Mouse Button Released
		MouseDeltaMoved, //!<Mouse Delta (Wheel) Moved
		TouchDown, //!<Touched Down (Pressed finger on screen)
		TouchUp, //!<Touched Up (Released finger from screen)
		TouchDrag, //!<Dragged Touch (Moved finger on screen)
		KeyPressed, //!<Key Pressed
		KeyReleased, //!<Key Released
		CharacterEntered, //!<Character Entered
		WindowGotFocus, //!<Window Gained Focus 
		WindowLostFocus, //!<Window Lost Focus
		WindowResized, //!<Window was Resized
		WindowClosed, //!<Window was Closed
		MouseLeft, //!<Mouse has left
		MouseEntered //!<Mouse has entered
	};
};

/*!
*	Renderer Event class
*/
class RendererEvent
{
public:
	/*!
	*	Event Type (one of RendererEventType::*)
	*/
	uint8 Type;

	/*!
	*	Mouse Position
	*/
	Vector2 MousePosition;

	/*!
	*	Window Size
	*/
	Vector2 WindowSize;

	/*!
	*	Mouse Wheel Delta
	*/
	f32 MouseDelta;

	/*!
	*	Joystick Axis Position [-1, 1]
	*/
	f32 JoystickAxisPosition;

	/*!
	*	Last Typed Character
	*/
	wchar_t TypedCharacter;

	/*!
	*	Current Keypress Keycode
	*/
	uint8 KeyCode;

	/*!
	*	Current Mouse Button Index
	*/
	uint8 MouseButtonIndex;

	/*!
	*	Joystick Axis Index
	*/
	uint8 JoystickAxisIndex;

	/*!
	*	Joystick Button Index
	*/
	uint8 JoystickButtonIndex;

	/*!
	*	Joystick Index
	*/
	uint8 JoystickIndex;

	/*!
	*	Touch Index
	*/
	uint8 TouchIndex;

	/*!
	*	Touch Position
	*/
	Vector2 TouchPosition;

	RendererEvent() : Type(0), MouseDelta(0), JoystickAxisPosition(0), TypedCharacter(0), KeyCode(0), MouseButtonIndex(0), JoystickAxisIndex(0), JoystickButtonIndex(0), JoystickIndex(0), TouchIndex(0) {};
};

/*!
*	Renderer Frame Statistics
*/
class RendererFrameStats
{
public:
	//!Total amount of draw calls this frame
	uint32 DrawCalls;
	
	//!Total amount of vertices drawn this frame
	uint32 VertexCount;
	
	//!Total amount of texture changes this frame
	uint32 TextureChanges;

	//!Total amount of matrix changes this frame
	uint32 MatrixChanges;

	//!Total amount of clipping changes this frame
	uint32 ClippingChanges;

	//!Total amount of state changes this frame
	uint32 StateChanges;

	//!Total amount of resources loaded
	uint32 TotalResources;

	//!Total amount of RAM used by resources (MB)
	f32 TotalResourceUsage;

	//!Skipped Draw Calls
	uint32 SkippedDrawCalls;

	//!Renderer Name
	std::string RendererName;

	//!Renderer Version
	std::string RendererVersion;

	//!Renderer Custom Message
	std::string RendererCustomMessage;

	RendererFrameStats() : DrawCalls(0), VertexCount(0), TextureChanges(0), MatrixChanges(0), ClippingChanges(0),
		StateChanges(0), TotalResources(0), TotalResourceUsage(0), SkippedDrawCalls(0) {};

	/*!
	*	Clears the statistics
	*/
	void Clear()
	{
		DrawCalls = VertexCount = TextureChanges = MatrixChanges = ClippingChanges =
			StateChanges = TotalResources = SkippedDrawCalls = 0;

		TotalResourceUsage = 0.0f;
	};
};

/*!
*	Renderer Capabilities container
*/
class RendererCapabilities
{
public:
	uint32 AntialiasLevel; //!<Anti-Aliasing Level
	uint32 DepthBits; //!<Depth Bits
	uint32 StencilBits; //!<Stencil Bits

	RendererCapabilities() : AntialiasLevel(0), DepthBits(32), StencilBits(0) {};
};

/*!
*	RendererDisplayMode
*	\note only 32 bpp displays are collected since lower than that is incredibly rare these days
*/
class RendererDisplayMode
{
public:
	//!Width of the display
	uint32 Width;
	//!Height of the display
	uint32 Height;
	//!Bits per Pixel
	uint32 Bpp;
};

class IRendererImplementation;

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
public:
	Camera RenderCamera;

	/*!
	*	OnFrameStarted should be used to do stuff before we render the main frame such as clearing the screen
	*	OnFrameEnded should be used to draw stuff after the frame has ended, such as our HUD
	*	OnFrameDraw should be used for drawing the main content of the frame
	*/
	SimpleDelegate::SimpleDelegate<Renderer *> OnFrameStarted, OnFrameEnded, OnFrameDraw;

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
	*	\return a vertex buffer handle, or 0 on error
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

/*!
*	Renderer Manager is a manager for multiple Renderers
*/
class RendererManager : public SubSystem
{
	friend class RendererInputProcessor;
public:
#if PROFILER_ENABLED
	Profiler::PacketMap ProfilerPackets;

	void OnGetProfilerPackets(const Profiler::PacketMap &Packets);
#endif
private:
	typedef std::map<RendererHandle, DisposablePointer<Renderer> > RendererMap;
	
	RendererMap Renderers;

public:
	bool ShowProfiler, ShowConsole;

private:
	std::string ConsoleText;
	uint32 ConsoleCursorPosition, ConsoleLogOffset;

	RendererManager();
	void StartUp(uint32 Priority);
	void Shutdown(uint32 Priority);
	void Update(uint32 Priority);
public:
	/*!
	*	The Input Processor
	*/
	InputCenter Input;

	static RendererManager Instance;

	/*!
	*	Add a renderer whose window will be created
	*	\param Title the Window Title
	*	\param Width the Renderer Width
	*	\param Height the Renderer Height
	*	\param Style the Window Style
	*	\param Caps the requested Renderer Capabilities
	*	\return a Renderer Handle. If it is 0, we failed to create the renderer
	*	\note After creation you must set the renderer as the active renderer if you don't have any set yet
	*/
	RendererHandle AddRenderer(const char *Title, uint32 Width, uint32 Height,
		uint32 Style = RendererWindowStyle::Default, RendererCapabilities Caps = RendererCapabilities());

	/*!
	*	Add a renderer to an existing window
	*	\param Window the Window Handle
	*	\param Caps the requested Renderer Capabilities
	*	\return a Renderer Handle. If it is 0, we failed to create the renderer
	*	\note After creation you must set the renderer as the active renderer if you don't have any set yet
	*/
	RendererHandle AddRenderer(void *Window, RendererCapabilities Caps = RendererCapabilities());

	/*!
	*	Destroy the currently active renderer
	*	\param Handle the Rendering Handle
	*/
	void DestroyRenderer(RendererHandle Handle);

	/*!
	*	Use a different renderer
	*	\param Handle the Renderer's Handle
	*/
	void SetActiveRenderer(RendererHandle Handle);

	/*!
	*	Get the active renderer
	*	\return the Active Renderer
	*/
	DisposablePointer<Renderer> ActiveRenderer();

	/*!
	*	Draws a frame (also checks input events)
	*	\return whether we should stop our app
	*/
	bool RequestFrame();

	/*!
	*	Gets the amount of renderers we already are managing
	*/
	uint32 RendererCount() const;
};

/*!
*	Renderer Implementation interface
*/
class IRendererImplementation
{
public:
	Renderer *Target;

	virtual ~IRendererImplementation() {};

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
	*	\param Character the character to get the glyph for
	*	\param Parameters the text parameters
	*	\return the Text Glyph Info of this glyph
	*/
	virtual TextGlyphInfo GetTextGlyph(uint32 Character, const TextParams &Parameters) = 0;

	/*!
	*	\param Prev the previous character
	*	\param Cur the current character
	*	\param Parameters the text parameters
	*	\return the kerning space between characters
	*/
	virtual int32 GetTextKerning(uint32 Prev, uint32 Cur, const TextParams &Parameters) = 0;

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
	*	Creates a Font from a Stream of Data
	*	\param Data the data of the font
	*	\return a FontHandle or 0xFFFFFFFF
	*/
	virtual FontHandle CreateFont(Stream *Data) = 0;

	/*!
	*	Destroys a font
	*	\param Handle the font's handle
	*/
	virtual void DestroyFont(FontHandle Handle) = 0;

	/*!
	*	Measures Text
	*	\param Text the text to measure
	*	\param Parameters the text parameters to use
	*	\return a rectangle with the text's bounds
	*/
	virtual Rect MeasureText(const std::string &Text, const TextParams &Parameters) = 0;

	/*!
	*	Renders Text to the screen
	*	\param Text the text to measure
	*	\param Parameters the text parameters to use
	*/
	virtual void RenderText(const std::string &Text, const TextParams &Parameters) = 0;

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

#if USE_SFML_RENDERER
#	include "SFMLRenderer.hpp"
#	undef DEFAULT_RENDERER_IMPLEMENTATION
#	define DEFAULT_RENDERER_IMPLEMENTATION SFMLRendererImplementation
#endif

#ifndef DEFAULT_RENDERER_IMPLEMENTATION
#	define DEFAULT_RENDERER_IMPLEMENTATION NULLRendererImplementation

class NULLRendererImplementation : public IRendererImplementation
{
public:
	/*!
	*	Creates a renderer from a Window Handle
	*	\param WindowHandle the window handle
	*	\param ExpectedCaps the expected capabilities of this renderer
	*/
	bool Create(void *WindowHandle, RendererCapabilities ExpectedCaps)
	{
		return true;
	};

	/*!
	*	Creates a renderer from window details
	*	\param Title the title of the window
	*	\param Width the width of the window
	*	\param Height the height of the window
	*	\param Style the style of the window (one of RendererWindowStyle::*)
	*	\param ExpectedCaps the expected capabilities of this renderer
	*/
	bool Create(const std::string &Title, uint32 Width, uint32 Height, uint32 Style, RendererCapabilities ExpectedCaps)
	{
		return true;
	};

	/*!
	*	\return the currently available renderer capabilities
	*/
	const RendererCapabilities &Capabilities() const
	{
		static RendererCapabilities caps;

		return caps;
	};

	/*!
	*	\return the Desktop Display Mode
	*/
	RendererDisplayMode DesktopDisplayMode()
	{
		return RendererDisplayMode();
	};

	/*!
	*	\return all available display modes
	*/
	std::vector<RendererDisplayMode> DisplayModes()
	{
		return std::vector<RendererDisplayMode>();
	};

	/*!
	*	Render Window Size
	*/
	Vector2 Size() const
	{
		return Vector2(1, 1);
	};

	/*!
	*	\param Character the character to get the glyph for
	*	\param Parameters the text parameters
	*	\return the Text Glyph Info of this glyph
	*/
	TextGlyphInfo GetTextGlyph(uint32 Character, const TextParams &Parameters)
	{
		static TextGlyphInfo Info;

		return Info;
	};

	/*!
	*	\param Prev the previous character
	*	\param Cur the current character
	*	\param Parameters the text parameters
	*	\return the kerning space between characters
	*/
	int32 GetTextKerning(uint32 Prev, uint32 Cur, const TextParams &Parameters)
	{
		return 0;
	};

	/*!
	*	Creates a Vertex Buffer
	*	\return a vertex buffer handle, or 0 on error
	*/
	VertexBufferHandle CreateVertexBuffer()
	{
		return 1;
	};

	/*!
	*	Checks whether a vertex buffer handle is valid
	*	\param Handle the Vertex Buffer Handle to check
	*/
	bool IsVertexBufferHandleValid(VertexBufferHandle Handle)
	{
		return 1;
	};

	/*!
	*	Sets a Vertex Buffer's Data
	*	\param Handle the Vertex Buffer's Handle
	*	\param DetailsMode one of VertexDetailsMode::*
	*	\param Elements a list of VertexElementDescriptor
	*	\param ElementCount the amount of VertexElements in Elements
	*	\param Data the data for the vertex buffer
	*	\param DataByteSize the byte size of the entire data
	*/
	void SetVertexBufferData(VertexBufferHandle Handle, uint8 DetailsMode, VertexElementDescriptor *Elements, uint32 ElementCount, const void *Data, uint32 DataByteSize) {};

	/*!
	*	Destroys a Vertex Buffer
	*	\param Handle the handle of the vertex buffer to destroy
	*/
	void DestroyVertexBuffer(VertexBufferHandle Handle) {};

	/*!
	*	Render vertices
	*	\param VertexMode one of VertexModes::*
	*	\param Buffer is the handle of the Vertex Buffer
	*	\param Start the starting vertex
	*	\param End the ending vertex
	*/
	void RenderVertices(uint32 VertexMode, VertexBufferHandle Buffer, uint32 Start, uint32 End) {};

	/*!
	*	Start clipping the rendering to a rectangle
	*	\param ClippingRect the clipping rectangle
	*	\note The orientation is always from bottom left and up, so e.g., to clip a 100x100 rect on the top right of a screen of 1000x1000px size, you must pass a rect of (900, 1000, 1000, 900)
	*	Therefore to keep up with the usual orientation we use (+y is "down" in the window), we emulate that orientation by flipping the top/bottom fields of Rect. So now the previous example becomes (900, 1000, 900, 1000)
	*/
	void StartClipping(const Rect &ClippingRect) {};

	/*!
	*	Finishes clipping the rendering from a rectangle
	*/
	void FinishClipping() {};

	/*!
	*	Clear a render buffer
	*	\param Buffers a Bitmask of RenderBuffer::*
	*/
	void Clear(uint32 Buffers) {};

	/*!
	*	Displays a frame
	*/
	void Display() {};

	/*!
	*	Gets the last Frame Statistics
	*/
	const RendererFrameStats &FrameStats() const
	{
		static RendererFrameStats stats;

		return stats;
	};

	/*!
	*	Sets the current World Matrix
	*	\param WorldMatrix the new World Matrix
	*/
	void SetWorldMatrix(const Matrix4x4 &WorldMatrix) {};

	/*!
	*	Sets the current Projection Matrix
	*	\param ProjectionMatrix the new World Matrix
	*/
	void SetProjectionMatrix(const Matrix4x4 &ProjectionMatrix) {};

	/*!
	*	Sets the current viewport
	*	\param x the viewport's x position
	*	\param y the viewport's y position
	*	\param Width the viewport's width
	*	\param Height the viewport's height
	*/
	void SetViewport(f32 x, f32 y, f32 Width, f32 Height) {};

	/*!
	*	Creates a texture
	*	\return a texture handle, or 0 on error
	*/
	TextureHandle CreateTexture()
	{
		return 1;
	};

	/*!
	*	Check whether a texture handle is still valid
	*	\param Handle the texture's handle
	*/
	bool IsTextureHandleValid(TextureHandle Handle)
	{
		return true;
	};

	/*!
	*	Destroys a texture
	*	\param Handle the texture handle to destroy
	*/
	void DestroyTexture(TextureHandle Handle) {};

	/*!
	*	Fills in a texture's data
	*	\param Handle the texture's handle
	*	\param Pixels an array of RGBA pixels
	*	\param Width the width of the texture
	*	\param Height the height of the texture
	*/
	void SetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 Width, uint32 Height) {};

	/*!
	*	Gets a texture's data to a pixel buffer
	*	\param Handle the texture's handle
	*	\param Pixels the pixels to store the screen to
	*	\param BufferByteCount the byte count of the buffer. It should always be equal to Width x Height x 4 bytes
	*	\return whether we were able to capture the screen
	*/
	bool GetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 BufferByteCount) { return true; };

	/*!
	*	Sets a texture's wrap mode
	*	\param Handle the texture's Handle
	*	\param WrapMode one of TextureWrapMode::*
	*/
	void SetTextureWrapMode(TextureHandle Handle, uint32 WrapMode) {};

	/*!
	*	Sets a texture's filter mode
	*	\param Handle the texture's Handle
	*	\param Filtering one of TextureFiltering::*
	*/
	void SetTextureFiltering(TextureHandle Handle, uint32 Filtering) {};

	/*!
	*	Sets the current blending mode
	*	\param BlendingMode one of BlendingMode::*
	*/
	void SetBlendingMode(uint32 BlendingMode) {};

	/*!
	*	Binds a texture for use
	*	\param Handle the texture's handle
	*/
	void BindTexture(TextureHandle Handle) {};

	/*!
	*	Captures the screen to a pixel buffer
	*	\param Pixels the pixels to store the screen to
	*	\param BufferByteCount the byte count of the buffer. It should always be equal to Width x Height x 4 bytes
	*	\return whether we were able to capture the screen
	*/
	bool CaptureScreen(uint8 *Pixels, uint32 BufferByteCount)
	{
		return true;
	};

	/*!
	*	Polls for new renderer events
	*	\param Out the RendererEvent we received
	*	\return true if there's a new event, false otherwise
	*/
	bool PollEvent(RendererEvent &Out)
	{
		return false;
	};

	/*!
	*	Creates a Font from a Stream of Data
	*	\param Data the data of the font
	*	\return a FontHandle or 0xFFFFFFFF
	*/
	FontHandle CreateFont(Stream *Data)
	{
		return 1;
	};

	/*!
	*	Destroys a font
	*	\param Handle the font's handle
	*/
	void DestroyFont(FontHandle Handle) {};

	/*!
	*	Measures Text
	*	\param Text the text to measure
	*	\param Parameters the text parameters to use
	*	\return a rectangle with the text's bounds
	*/
	Rect MeasureText(const std::string &Text, const TextParams &Parameters)
	{
		return Rect();
	};

	/*!
	*	Renders Text to the screen
	*	\param Text the text to measure
	*	\param Parameters the text parameters to use
	*/
	void RenderText(const std::string &Text, const TextParams &Parameters) {};

	/*!
	*	Gets the Native Window Handle of this Renderer
	*/
	void *WindowHandle() const
	{
		return NULL;
	};

	/*!
	*	Sets the current Mouse Position
	*	\param Position the new Mouse Position
	*/
	void SetMousePosition(const Vector2 &Position) {};

	/*!
	*	Sets the Rendering Frame Rate
	*	\param FPS the target FPS
	*/
	void SetFrameRate(uint32 FPS) {};

	/*!
	*	Reports a Skipped Draw Call (used to help figure out how much we're optimizing by skipping unnecessary drawcalls)
	*/
	void ReportSkippedDrawCall() {};
};

#endif

