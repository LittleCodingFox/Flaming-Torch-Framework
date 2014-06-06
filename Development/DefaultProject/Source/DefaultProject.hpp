#ifndef __DEFAULTPROJECT_HPP__
#define __DEFAULTPROJECT_HPP__

#define _CRT_SECURE_NO_WARNINGS

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
