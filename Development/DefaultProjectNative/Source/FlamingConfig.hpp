#define USE_GRAPHICS 1
#define USE_SOUND 1
#define USE_SFML_RENDERER 1
#define USE_SFML_SOUNDMANAGER 1

//Set this to 1 when preparing a release build
#define FLGAME_RELEASE 0

//Set this to 1 if you want a Sandbox Build (will store files on the same folder as the app)
#define SANDBOX_BUILD !FLGAME_RELEASE

//Whether we want to debug UI Input Events (outputs to log)
#define DEBUGGING_UI_INPUT 0

//Set this to 1 if you want to verify all AsDerived SSP conversions
#define CHECK_SSP_CONVERSIONS !FLGAME_RELEASE

#define PROFILER_ENABLED !FLGAME_RELEASE
