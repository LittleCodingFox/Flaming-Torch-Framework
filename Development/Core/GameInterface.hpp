#pragma once

#if USE_GRAPHICS
struct RenderCreateOptions
{
	std::string Title;
	uint32 Width, Height;
	uint32 Style;
	uint32 FrameRate;
	void *WindowHandle;
	RendererCapabilities Caps;

	RenderCreateOptions() : Style(RendererWindowStyle::Default), WindowHandle(NULL), FrameRate(0), Width(0), Height(0) {};
};
#endif

class GameInterface : public LuaLib
{
	friend class NativeGameInterface;
	friend class ScriptedGameInterface;
private:
#if USE_GRAPHICS
	void OnGUISandboxTrigger(const std::string &Directory, const std::string &FileName, uint32 Action);
#endif
public:
	bool DevelopmentBuild;
	bool IsGUISandbox;
	bool IsMobile;
	uint32 UpdateRateValue, FrameRateValue;
	Vector2 BaseResolution;

	static SuperSmartPointer<GameInterface> Instance;

	GameInterface() : DevelopmentBuild(false), IsGUISandbox(false), UpdateRateValue(30), FrameRateValue(0) {};
	virtual ~GameInterface() {};

	static void SetInstance(SuperSmartPointer<GameInterface> TheInstance);

	/*!
	*	Load a package from a filename
	*/
	bool LoadPackage(const std::string &PackageName);

	/*!
	*	The game's name
	*/
	virtual const std::string &GameName() = 0;
	/*!
	*	Register any scripting bindings here
	*/
	virtual bool Register(lua_State *State) { return true; };

	virtual int32 FixedUpdateRate() = 0;
	/*!
	*	Run your initialization code here
	*	\param argc argument count
	*	\param argv argument values
	*/
	virtual bool Initialize(int32 argc, char **argv) { return true; };
	/*!
	*	Run your cleanup code here
	*/
	virtual bool DeInitialize() { return true; };
	/*!
	*	Run your fixed step update code here
	*/
	virtual void OnFixedUpdate() {};
	/*!
	*	Run your frame update code here
	*/
	virtual void OnFrameUpdate() {};
	/*!
	*	Whether we should quit
	*/
	virtual bool ShouldQuit() { return false; };

	/*!
	*	Gets any script instances available for this game
	*	Used to interact with the game's scripting or methods if you have any
	*/
	virtual SuperSmartPointer<LuaScript> GetScriptInstance() { return SuperSmartPointer<LuaScript>(); };

#if USE_GRAPHICS
	/*!
	*	Used by the UI Sandbox
	*	Tries to get the GUILayout.resource file from the game's directory and load it
	*/
	void ReloadGUI();

	/*!
	*	Creates a renderer from options
	*	\param Options the options to create the renderer
	*	\return The Renderer that was created, or NULL
	*/
	Renderer *CreateRenderer(const RenderCreateOptions &Options);

	/*!
	*	Called when a frame begins
	*	\param TheRenderer the renderer to use for rendering
	*/
	virtual void OnFrameBegin(Renderer *TheRenderer) {};

	/*!
	*	Called when a frame is drawn
	*	\param TheRenderer the renderer to use for rendering
	*/
	virtual void OnFrameDraw(Renderer *TheRenderer) {};

	/*!
	*	Called when a frame ends
	*	\param TheRenderer the renderer to use for rendering
	*/
	virtual void OnFrameEnd(Renderer *TheRenderer);

	/*!
	*	Called when a renderer resizes
	*	\param TheRenderer the renderer to use for rendering
	*	\param Width the new width
	*	\param Height the new height
	*/
	virtual void OnResize(Renderer *TheRenderer, uint32 Width, uint32 Height) {};

	/*!
	*	Called when resources have been reloaded
	*	\param TheRenderer the renderer to use for rendering
	*/
	virtual void OnResourcesReloaded(Renderer *TheRenderer) {};
#endif

	/*!
	*	Runs the main loop
	*	\param argc the argument count
	*	\param argv the argument values
	*/
	virtual int32 Run(int32 argc, char **argv) = 0;
};

class NativeGameInterface : public GameInterface
{
public:
	int32 Run(int32 argc, char **argv);
};

class ScriptedGameInterface : public GameInterface
{
public:
	SuperSmartPointer<LuaScript> ScriptInstance;
	std::string GameNameValue;

	luabind::object PreInitFunction, InitFunction, DeInitFunction, OnFixedUpdateFunction, OnFrameUpdateFunction,
		OnFrameBeginFunction, OnFrameEndFunction, OnFrameDrawFunction, OnResizeFunction, OnResourcesReloadedFunction,
		ShouldQuitFunction;

	const std::string &GameName()
	{
		return GameNameValue;
	};

	int32 FixedUpdateRate()
	{
		return UpdateRateValue;
	};

	ScriptedGameInterface() : GameNameValue("Game")
	{
	};

	~ScriptedGameInterface();

	SuperSmartPointer<LuaScript> GetScriptInstance()
	{
		return ScriptInstance;
	};

#if USE_GRAPHICS
	void OnFrameBegin(Renderer *TheRenderer);
	void OnFrameDraw(Renderer *TheRenderer);
	void OnFrameEnd(Renderer *TheRenderer);
	void OnResize(Renderer *TheRenderer, uint32 Width, uint32 Height);
	void OnResourcesReloaded(Renderer *TheRenderer);
#endif

	void OnFixedUpdate();
	void OnFrameUpdate();
	bool ShouldQuit();

	int32 Run(int32 argc, char **argv);
};

