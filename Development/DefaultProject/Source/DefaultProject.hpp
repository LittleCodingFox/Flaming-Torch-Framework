#ifndef __DEFAULTPROJECT_HPP__
#define __DEFAULTPROJECT_HPP__

//Lousy Win32 defs...
#ifdef CreateDirectory
#undef CreateDirectory
#endif

#ifdef SetMenu
#undef SetMenu
#endif

#define VERSION_MAJOR 0
#define VERSION_MINOR 1

#define GAME_WINDOW_WIDTH 960
#define GAME_WINDOW_HEIGHT 600

namespace FlamingTorch
{
#	define TAG "DefaultProject"

	class DefaultProject : public ScriptedGameInterface
	{
	public:
		bool Register(lua_State *State) { return true; };
	};
};

#endif
