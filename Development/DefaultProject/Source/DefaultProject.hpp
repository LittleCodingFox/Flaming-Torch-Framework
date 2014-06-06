#ifndef __DEFAULTPROJECT_HPP__
#define __DEFAULTPROJECT_HPP__

#define _CRT_SECURE_NO_WARNINGS

//General includes
#include "FlamingConfig.hpp"
#include "CoreVersion.hpp"
#include <vector>
#include <map>
#include <list>
#include <sstream>
#include <GL/glew.h>
#include <enet/enet.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <json/json.h>

extern "C"
{
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}
#include <luabind/luabind.hpp>
#include <luabind/class.hpp>
#include <luabind/function.hpp>
#include <luabind/operator.hpp>

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
#	include "SubSystem.hpp"
#	include "Log.hpp"
#	include "StringID.hpp"
#	include "GameClock.hpp"
#	include "SuperSmartPointer.hpp"
#	include "FileSystem.hpp"
#	include "Math.hpp"
#	include "GenericConfig.hpp"
#	include "Shader.hpp"
#	include "Texture.hpp"
#	include "Input.hpp"
#	include "Profiler.hpp"
#	include "Camera.hpp"
#	include "RenderBase.hpp"
#	include "LuaScript.hpp"
#	include "GameInterface.hpp"
#	include "Sprite.hpp"
#	include "RenderTextUtils.hpp"
#	include "GUI.hpp"
#	include "ResourceManager.hpp"
#	include "FPSCounter.hpp"
#	include "Future.hpp"
#	include "SoundManager.hpp"
#	include "StringUtils.hpp"
#	include "MiniDump.hpp"
#	include "Randomizer.hpp"
#	include "TiledMap.hpp"
#
#	define TAG "DefaultProject"

	class DefaultProject : public ScriptedGameInterface
	{
	public:
		bool Register(lua_State *State) { return true; };
	};
};

#endif
