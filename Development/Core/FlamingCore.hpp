#include "FlamingConfig.hpp"
#include "CoreVersion.hpp"
#include <vector>
#include <map>
#include <list>
#include <sstream>
#include "MaxRectsBinPack.h"
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
#	include <json/json.h>
#
#if USE_NETWORK
#	include <enet/enet.h>
#endif
#
#	include <SFML/System.hpp>
#
#if USE_SOUND
#	include <SFML/Audio.hpp>
#endif
namespace FlamingTorch
{
#	undef DrawText
#	undef CopyDirectory
#	undef CreateDirectory

	typedef uint64 FontHandle, TextureHandle, VertexBufferHandle, RendererHandle;

#	include "MiniDump.hpp"
#	include "SubSystem.hpp"
#	include "Log.hpp"
#	include "GameClock.hpp"
#	include "StringID.hpp"
#	include "CRC32.hpp"
#	include "Profiler.hpp"
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
#	include "Randomizer.hpp"
#	include "Camera.hpp"
#	include "DynamicLink.hpp"
#
#	if USE_GRAPHICS
#		include "Input.hpp"
#		include "RenderTextUtils.hpp"
#		include "RenderBase.hpp"
#		include "GameInterface.hpp"
#		include "GenericConfig.hpp"
#		include "Sprite.hpp"
#		include "UIUtils.hpp"
#		include "UIElement.hpp"
#		include "UIText.hpp"
#		include "UIGroup.hpp"
#		include "UISprite.hpp"
#		include "UITextComposer.hpp"
#		include "UILayout.hpp"
#		include "UIManager.hpp"
#	endif
#
#	include "Texture.hpp"
#	include "ResourceManager.hpp"
#	include "TextureCollision.hpp"
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
#	include "ObjectModel.hpp"
};
