#define USE_GRAPHICS 0
#define USE_ANGELSCRIPT 0
#define USE_LUA 0
#define USE_NETWORK 0
#define USE_BOX2D 0
#define USE_AWESOMIUM 0
#define USE_SOUND 0

//Set this to 1 when preparing a release build
#define FLGAME_RELEASE 0

//Set this to 1 if you want a Sandbox Build (will store files on the same folder as the app)
#define SANDBOX_BUILD 1

//Set this to 1 if you're experiencing memory corruption such as double-frees related to SuperSmartPointers
//So it will log how each SSP is created and destroyed, and detect double creations of the same item (in debug builds)
#if ULTIMATE_DEBUG_MODE
#	define LOG_SSP_BEHAVIOUR 1
#else
#	define LOG_SSP_BEHAVIOUR 0
#endif

//Enable this if experiencing extreme FPS loss to detect a bottleneck related to SSPs being copied around each frame
#define LOG_SSP_ALLOCS 0

//Set this to 1 if you want to verify all AsDerived SSP conversions
#define CHECK_SSP_CONVERSIONS !FLGAME_RELEASE

#define AWESOMIUM_SOUND_ENABLED 0

#define PROFILER_ENABLED !FLGAME_RELEASE

#if !USE_GRAPHICS
#	define GLCHECK()
#else
#	define GLCHECK() { int32 error = glGetError(); if(error != GL_NO_ERROR) { FLASSERT(0, "GL Error %08x!", error); } }
#endif
