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

	RenderCreateOptions() : Style(RendererWindowStyle::Default), WindowHandle(NULL), FrameRate(0), Width(0), Height(0) {}
};
#endif

class GameInterface
{
	friend class NativeGameInterface;
private:
#if USE_GRAPHICS
	void OnGUISandboxTrigger(const std::string &Directory, const std::string &FileName, uint32 Action);
#endif
public:
	bool QuitFlag;
	bool GraphicsEnabled;
	bool DevelopmentBuild;
	bool IsGUISandbox;
	uint32 UpdateRateValue, FrameRateValue;
	Vector2 BaseResolution;

	GameInterface() : DevelopmentBuild(false), IsGUISandbox(false), UpdateRateValue(30), FrameRateValue(0), GraphicsEnabled(USE_GRAPHICS), QuitFlag(false) {}

	virtual ~GameInterface() {}

	/*!
	*	Load a package from a filename
	*/
	bool LoadPackage(const std::string &PackageName);

	/*!
	*	The game's name
	*/
	virtual const std::string &GameName() = 0;

	virtual int32 FixedUpdateRate() = 0;
	/*!
	*	Run your initialization code here
	*	\param argc argument count
	*	\param argv argument values
	*/
	virtual bool Initialize(int32 argc, char **argv) { return true; }
	/*!
	*	Run your cleanup code here
	*/
	virtual bool DeInitialize() { return true; }
	/*!
	*	Run your fixed step update code here
	*/
	virtual void OnFixedUpdate();
	/*!
	*	Run your frame update code here
	*/
	virtual void OnFrameUpdate();
	/*!
	*	Whether we should quit
	*/
	virtual bool ShouldQuit() { return QuitFlag; }

#if USE_GRAPHICS
	/*!
	*	Used by the UI Sandbox
	*	Tries to get the GUILayout.resource file from the game's directory and load it
	*/
	void ReloadGUI();

	/*!
	*	Creates a renderer from options
	*	\param Options the options to create the renderer
	*/
	bool CreateRenderer(const RenderCreateOptions &Options);

	/*!
	*	Called when a frame begins
	*/
	virtual void OnFrameBegin() {}

	/*!
	*	Called when a frame is drawn
	*/
	virtual void OnFrameDraw() {}

	/*!
	*	Called when a frame ends
	*/
	virtual void OnFrameEnd() {}

	/*!
	*	Called when a renderer resizes
	*	\param Width the new width
	*	\param Height the new height
	*/
	virtual void OnResize(uint32 Width, uint32 Height) {}

	/*!
	*	Called when resources have been reloaded
	*/
	virtual void OnResourcesReloaded() {}
#endif

	/*!
	*	Runs the main loop
	*	\param argc the argument count
	*	\param argv the argument values
	*/
	virtual int32 Run(int32 argc, char **argv);
};

class NativeGameInterface : public GameInterface
{
public:
	int32 Run(int32 argc, char **argv) override;
};

void GameInterfaceSingleFrame();

extern DisposablePointer<GameInterface> g_Game;
