#if USE_GRAPHICS
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
#endif
