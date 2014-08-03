#pragma once
typedef uint32 RendererHandle;
class InputCenter;
class Texture;
class UIManager;

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

/*!
*	Renderer Manager is a manager for multiple Renderers
*/
class FLAMING_API RendererManager : public SubSystem
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
	private:
		bool AnisotropicSupported, AnisotropicEnabled;
		f32 MaxAnisotropicLevel, CurrentAnisotropicLevel;
		bool CanAutoGenerateMipMaps;
		bool MayUseNonPOTTextures;

		Renderer() : AnisotropicSupported(false), AnisotropicEnabled(false),
			MaxAnisotropicLevel(1), CurrentAnisotropicLevel(1), CanAutoGenerateMipMaps(false),
			GlewIsAvailable(true), MayUseNonPOTTextures(false) {};

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
