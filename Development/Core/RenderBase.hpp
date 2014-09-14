#pragma once
typedef uint32 RendererHandle;
class InputCenter;
class Texture;
class UIManager;

/*!
*	Renderer Vertex Modes namespace
*/
namespace VertexModes
{
	enum
	{
		Triangles, //!<Triangle list
		Lines, //!<Line list
		Points //!<Point list
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
*	Renderer Features namespace
*/
namespace RendererFeature
{
	enum
	{
		GlewIsAvailable = 0, //!< Whether GLEW is available (Bool)
		AnisotropicSupported, //!<Whether Anisotropic Filtering is supported (Bool)
		AnisotropicEnabled, //!<Whether Anisotropic Filtering is Enabled (Bool)
		MaxAnisotropicLevel, //!<The Maximum Anisotropic Level (f32)
		CurrentAnisotropicLevel, //!<The Current Anisotropic Level (f32)
		CanAutoGenerateMipMaps, //!<Whether we can auto generate MipMaps (Bool)
		MayUseNonPowerOfTwoTextures, //!<Whether we may use Non Power of Two Texture sizes (Bool)
		Count //!<Unused, dont use this.
	};
};

typedef uint32 TextureHandle;

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

/*!
*	Renderer Implementation interface
*/
class IRendererImplementation
{
public:
	virtual ~IRendererImplementation() {};

	/*!
	*	Render Window Size
	*/
	virtual const Vector2 &Size() const = 0;

	/*!
	*	Render vertices
	*	\param VertexMode one of VertexModes::*
	*	\param VertexData the vertex data as a pile of bytes
	*	\param Start the starting vertex
	*	\param End the ending vertex
	*/
	virtual void RenderVertices(uint32 VertexMode, void *VertexData, uint32 Start, uint32 End) = 0;

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
	*	Sets the current viewport
	*	\param x the viewport's x position
	*	\param y the viewport's y position
	*	\param Width the viewport's width
	*	\param Height the viewport's height
	*/
	virtual void SetViewport(f32 x, f32 y, f32 Width, f32 Height) = 0;

	/*!
	*	Creates a texture
	*	\return a texture handle, or 0xFFFFFFFF on error
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
	*	Binds a texture for use
	*	\param Handle the texture's handle
	*/
	virtual void BindTexture(TextureHandle Handle) = 0;

	/*!
	*	Polls for new renderer events
	*	\param Out the RendererEvent we received
	*	\return true if there's a new event, false otherwise
	*/
	virtual bool PollEvent(RendererEvent &Out);
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
		bool AnisotropicSupported, AnisotropicEnabled;
		f32 MaxAnisotropicLevel, CurrentAnisotropicLevel;
		bool CanAutoGenerateMipMaps;
		bool MayUseNonPOTTextures;

		Renderer() : AnisotropicSupported(false), AnisotropicEnabled(false),
			MaxAnisotropicLevel(1), CurrentAnisotropicLevel(1), CanAutoGenerateMipMaps(false),
			GlewIsAvailable(true), MayUseNonPOTTextures(false), Impl(NULL) {};

		Renderer(const Renderer &o);
		Renderer &operator=(const Renderer &o);
		RendererHandle HandleValue;

		/*!
		*	OpenGL Rendering States Map
		*	Used to prevent unnecessary State Changes
		*/
		typedef std::map<uint32, bool> GLStatesMap;
		GLStatesMap GLStates;

		/*!
		*	Last Bound Texture pointer
		*	Used to prevent unnecessary Texture Changes
		*/
		Texture *LastBoundTexture;

		/*!
		*	FullScreen Flag
		*/
		bool FullScreenFlag;

		/*!
		*	Window Title
		*/
		sf::String WindowTitle;

		/*!
		*	Window Style
		*/
		uint32 WindowStyle;

		/*!
		*	Whether the Renderer was created from a Window
		*/
		bool CreatedFromWindow;

		/*!
		*	Does resizing. Use with care.
		*/
		void HandleResize(uint32 w, uint32 h);
	public:
		Camera RenderCamera;

		bool GlewIsAvailable;

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
		*	Rendering window
		*/
		sf::RenderWindow Window;

		/*!
		*	UI Manager
		*/
		SuperSmartPointer<UIManager> UI;

		/*!
		*	Returns the value of a Boolean feature
		*	\param ID one of RendererFeature::*
		*	\sa RendererFeature
		*/
		bool GetBoolFeature(uint32 ID);
		/*!
		*	Returns the value of a Float feature
		*	\param ID one of RendererFeature::*
		*	\sa RendererFeature
		*/
		f32 GetFloatFeature(uint32 ID);
		/*!
		*	Returns the value of a Uint feature
		*	\param ID one of RendererFeature::*
		*	\sa RendererFeature
		*/
		uint32 GetUintFeature(uint32 ID);

		/*!
		*	\return the Renderer Handle
		*/
		RendererHandle Handle() const;

		/*!
		*	Clears the framebuffer in some way
		*	\param ID the GL IDs (like GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
		*/
		void Clear(uint32 ID);
		
		/*!
		*	Sets the FullScreen Flag
		*	\param TargetResolution the resolution we want to use
		*	\param Value the value to set the fullscreen mode to
		*	\return Whether we succeeded in going fullscreen
		*	\note if it fails, it's more likely that the window is now broken.
		*	\note You should now either recreate it again in the old resolution or quit the app
		*/
		bool SetFullScreen(const Vector2 &TargetResolution, bool Value);

		/*!
		*	Gets the FullScreen Flag value
		*/
		bool GetFullScreen() const;

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
		*	Binds a texture
		*	\param t the Texture to bind
		*	\note Pass NULL to unbind any textures
		*/
		void BindTexture(Texture *t);

		/*!
		*	Returns the window size
		*/
		Vector2 Size() const;
	};
private:
	typedef std::map<uint32, SuperSmartPointer<Renderer> > RendererMap;
	
	RendererMap Renderers;

public:
	bool ShowProfiler, ShowConsole;

private:
	sf::String ConsoleText;
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
		uint32 Style = sf::Style::Resize | sf::Style::Close);
	/*!
	*	Add a renderer to an existing window
	*	\param Window the Window Handle
	*	\return a Renderer Handle. If it is 0, we failed to create the renderer
	*	\note After creation you must set the renderer as the active renderer if you don't have any set yet
	*/
	RendererHandle AddRenderer(sf::WindowHandle Window);
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
};
