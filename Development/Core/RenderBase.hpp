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
	enum
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
	enum
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
	enum
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
	enum
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
	enum
	{
		Popup = FLAGVALUE(0), //!<Window is a popup with no border or title bar
		FullScreen = FLAGVALUE(1), //!<Window is a fullscreen window
		Default = FLAGVALUE(2) //!<Default Window with a border and title bar
	};
};

/*!
*	Renderer Buffers namespace
*/
namespace RendererBuffer
{
	enum
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
	enum
	{
		JoystickConnected,
		JoystickDisconnected,
		JoystickButtonPressed,
		JoystickButtonReleased,
		JoystickAxisMoved,
		MouseMoved,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseDeltaMoved,
		KeyPressed,
		KeyReleased,
		CharacterEntered,
		WindowGotFocus,
		WindowLostFocus,
		WindowResized,
		WindowClosed,
		MouseLeft,
		MouseEntered
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
private:
	IRendererImplementation *Impl;

	Renderer(IRendererImplementation *_Impl);
	Renderer(const Renderer &o);
	Renderer &operator=(const Renderer &o);
	RendererHandle HandleValue;
public:
	Camera RenderCamera;

	/*!
	*	OnFrameStarted should be used to do stuff before we render the main frame such as clearing the screen
	*	OnFrameEnded should be used to draw stuff after the frame has ended, such as our HUD
	*	OnFrameDraw should be used for drawing the main content of the frame
	*/
	Signal1<Renderer *> OnFrameStarted, OnFrameEnded, OnFrameDraw;

	/*!
	*	OnResized should be used to reset your camera and projection matrix
	*	Parameters are Renderer, Width, Height
	*/
	Signal3<Renderer *, uint32, uint32> OnResized;

	/*!
	*	On Resources Reloaded should be used to reset any states that are not usually saved by the renderer
	*	Might be removed later on
	*/
	Signal1<Renderer *> OnResourcesReloaded;

	/*!
	*	UI Manager
	*/
	SuperSmartPointer<UIManager> UI;

	/*!
	*	\return the Renderer Handle
	*/
	RendererHandle Handle() const;

	/*!
	*	Creates a renderer from a Window Handle
	*/
	bool Create(void *WindowHandle);

	/*!
	*	Creates a renderer from window details
	*	\param Title the title of the window
	*	\param Width the width of the window
	*	\param Height the height of the window
	*	\param Style the style of the window (one or more of RendererWindowStyle::*)
	*/
	bool Create(const std::string &Title, uint32 Width, uint32 Height, uint32 Style);

	/*!
	*	Render Window Size
	*/
	Vector2 Size() const;
	
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
	*	Clear a render buffer
	*	\param Buffers a Bitmask of RenderBuffer::*
	*/
	void Clear(uint32 Buffers);

	/*!
	*	Displays a frame
	*/
	void Display();

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
	*	\param Handle the font's handle
	*	\param Text the text to measure
	*	\param Parameters the text parameters to use
	*	\return a rectangle with the text's bounds
	*/
	Rect MeasureText(FontHandle Handle, const std::string &Text, const TextParams &Parameters);

	/*!
	*	Renders Text to the screen
	*	\param Handle the font's handle
	*	\param Text the text to measure
	*	\param Parameters the text parameters to use
	*/
	void RenderText(FontHandle Handle, const std::string &Text, const TextParams &Parameters);

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
	typedef std::map<RendererHandle, SuperSmartPointer<Renderer> > RendererMap;
	
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
	*	\return a Renderer Handle. If it is 0, we failed to create the renderer
	*	\note After creation you must set the renderer as the active renderer if you don't have any set yet
	*/
	RendererHandle AddRenderer(const char *Title, uint32 Width, uint32 Height,
		uint32 Style = RendererWindowStyle::Default);

	/*!
	*	Add a renderer to an existing window
	*	\param Window the Window Handle
	*	\return a Renderer Handle. If it is 0, we failed to create the renderer
	*	\note After creation you must set the renderer as the active renderer if you don't have any set yet
	*/
	RendererHandle AddRenderer(void *Window);

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
	Renderer *ActiveRenderer();

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
	*/
	virtual bool Create(void *WindowHandle) = 0;

	/*!
	*	Creates a renderer from window details
	*	\param Title the title of the window
	*	\param Width the width of the window
	*	\param Height the height of the window
	*	\param Style the style of the window (one or more of RendererWindowStyle::*)
	*/
	virtual bool Create(const std::string &Title, uint32 Width, uint32 Height, uint32 Style) = 0;

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
	*	Render vertices
	*	\param VertexMode one of VertexModes::*
	*	\param Buffer is the handle of the Vertex Buffer
	*	\param Start the starting vertex
	*	\param End the ending vertex
	*/
	virtual void RenderVertices(uint32 VertexMode, VertexBufferHandle Buffer, uint32 Start, uint32 End) = 0;

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
	*	Pushes (stores) the currently used world and projection matrices
	*/
	virtual void PushMatrices() = 0;

	/*!
	*	Pops (restores) the last used world and projection matrices
	*/
	virtual void PopMatrices() = 0;

	/*!
	*	Gets the current World Matrix
	*/
	virtual const Matrix4x4 &WorldMatrix() = 0;

	/*!
	*	Gets the current projection matrix
	*/
	virtual const Matrix4x4 &ProjectionMatrix() = 0;

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
	*	\param Handle the font's handle
	*	\param Text the text to measure
	*	\param Parameters the text parameters to use
	*	\return a rectangle with the text's bounds
	*/
	virtual Rect MeasureText(FontHandle Handle, const std::string &Text, const TextParams &Parameters) = 0;

	/*!
	*	Renders Text to the screen
	*	\param Handle the font's handle
	*	\param Text the text to measure
	*	\param Parameters the text parameters to use
	*/
	virtual void RenderText(FontHandle Handle, const std::string &Text, const TextParams &Parameters) = 0;

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
	*/
	bool Create(void *WindowHandle)
	{
		return true;
	};

	/*!
	*	Creates a renderer from window details
	*	\param Title the title of the window
	*	\param Width the width of the window
	*	\param Height the height of the window
	*	\param Style the style of the window (one or more of RendererWindowStyle::*)
	*/
	bool Create(const std::string &Title, uint32 Width, uint32 Height, uint32 Style)
	{
		return true;
	};

	/*!
	*	Render Window Size
	*/
	Vector2 Size() const
	{
		return Vector2(1, 1);
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
	*	Clear a render buffer
	*	\param Buffers a Bitmask of RenderBuffer::*
	*/
	void Clear(uint32 Buffers) {};

	/*!
	*	Displays a frame
	*/
	void Display() {};

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
	*	Pushes (stores) the currently used world and projection matrices
	*/
	void PushMatrices() {};

	/*!
	*	Pops (restores) the last used world and projection matrices
	*/
	void PopMatrices() {};

	/*!
	*	Gets the current World Matrix
	*/
	const Matrix4x4 &WorldMatrix()
	{
		static Matrix4x4 Dummy;

		return Dummy;
	};

	/*!
	*	Gets the current projection matrix
	*/
	const Matrix4x4 &ProjectionMatrix()
	{
		static Matrix4x4 Dummy;

		return Dummy;
	};

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
	*	\param Handle the font's handle
	*	\param Text the text to measure
	*	\param Parameters the text parameters to use
	*	\return a rectangle with the text's bounds
	*/
	Rect MeasureText(FontHandle Handle, const std::string &Text, const TextParams &Parameters)
	{
		return Rect();
	};

	/*!
	*	Renders Text to the screen
	*	\param Handle the font's handle
	*	\param Text the text to measure
	*	\param Parameters the text parameters to use
	*/
	void RenderText(FontHandle Handle, const std::string &Text, const TextParams &Parameters) {};

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
};

#endif

