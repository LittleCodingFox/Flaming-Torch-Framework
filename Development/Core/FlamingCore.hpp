#include "FlamingConfig.hpp"
#include "CoreVersion.hpp"
#include <vector>
#include <map>
#include <list>
#include <sstream>
#
extern "C"
{
#	include <lua/lua.h>
#	include <lua/lauxlib.h>
#	include <lua/lualib.h>
}
#	include <luabind/luabind.hpp>
#	include <luabind/class.hpp>
#	include <luabind/function.hpp>
#	include <luabind/operator.hpp>
#	include <luabind/out_value_policy.hpp>
#	include <luabind/return_reference_to_policy.hpp>
#
#if USE_GRAPHICS
#	include <GL/glew.h>
#	include <SFML/Graphics.hpp>
#	include <json/json.h>
#else
#	include <SFML/System.hpp>
#endif
#
#if USE_NETWORK
#	include <enet/enet.h>
#endif
#
#if USE_SOUND
#	include <SFML/Audio.hpp>
#endif
namespace FlamingTorch
{
#	include "SubSystem.hpp"
#	include "Log.hpp"
#	include "GameClock.hpp"
#	include "StringID.hpp"
#	include "CRC32.hpp"
#	include "Profiler.hpp"
#	include "SuperSmartPointer.hpp"
#	include "FileSystem.hpp"
#	include "Math.hpp"
#	include "FrustumCuller.hpp"
#	include "DijkstraField.hpp"
#	include "FPSCounter.hpp"
#	include "FileSystem.hpp"
#	include "LuaScript.hpp"
#	include "GenericConfig.hpp"
#	include "StringUtils.hpp"
#	include "Future.hpp"
#	include "Entity.hpp"
#	include "Component.hpp"
#	include "Texture.hpp"
#	include "Randomizer.hpp"
#	include "ResourceManager.hpp"
#	include "Camera.hpp"
#	include "DynamicLink.hpp"
#
#	if USE_GRAPHICS
#		include "Shader.hpp"
#		include "Input.hpp"
#		include "RenderBase.hpp"
#		include "GameInterface.hpp"
#		include "GenericConfig.hpp"
#		include "Sprite.hpp"
#		include "RenderTextUtils.hpp"
#		include "GUI.hpp"
#		include "GeometryBuffer.hpp"
#		include "RenderTarget.hpp"
#		include "TextureCollision.hpp"
#	endif
#
#	include "GameInterface.hpp"
#	include "Console.hpp"
#
#	if USE_NETWORK
#		include "Network.hpp"
#	endif
#
#	if USE_SOUND
#		include "SoundManager.hpp"
#	endif
#
#	include "TiledMap.hpp"
#	include "PerlinNoise.hpp"
#	include "State.hpp"
#	include "WorldStreamer.hpp"
};
