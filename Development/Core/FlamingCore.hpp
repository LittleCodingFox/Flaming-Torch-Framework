#include "FlamingConfig.hpp"
#
#ifdef EMSCRIPTEN
#	include <emscripten.h>
#endif
#
#include "System/CoreVersion.hpp"
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
#include <vector>
#include <map>
#include <list>
#include <sstream>
#include <thread>
#include <mutex>
#include <algorithm>
#include "MaxRectsBinPack.h"
#include <json/json.h>
#include <ft2build.h>
#include FT_FREETYPE_H
namespace FlamingTorch
{
#	undef DrawText
#	undef CopyDirectory
#	undef CreateDirectory
#	define INVALID_FTGHANDLE 0

	typedef uint64 TextureHandle, VertexBufferHandle, SoundHandle, MusicHandle;

#	include "System/DisposablePointer.hpp"
#	include "System/MiniDump.hpp"
#	include "System/SubSystem.hpp"
#	include "System/Log.hpp"
#	include "System/StringID.hpp"
#	include "System/CRC32.hpp"
#	include "System/Profiler.hpp"
#	include "System/FileSystem.hpp"
#	include "System/Math.hpp"
#	include "System/GenericConfig.hpp"
#	include "System/StringUtils.hpp"
#	include "Game/GameClock.hpp"
#	include "System/Future.hpp"
#	include "System/Randomizer.hpp"
#	include "System/DynamicLink.hpp"
#
#	include "Rendering/Texture.hpp"
#
#	include "Rendering/FPSCounter.hpp"
#	include "Rendering/Camera.hpp"
#
#	include "Audio/SoundManager.hpp"
#
#	include "Rendering/Input.hpp"
#	include "Rendering/Text.hpp"
#	include "Rendering/RenderTextUtils.hpp"
#
#	include "Scripting/LuaScript.hpp"
#
#	include "Rendering/UI/UIUtils.hpp"
#	include "Rendering/UI/UIElement.hpp"
#	include "Rendering/UI/UIManager.hpp"
#	include "Rendering/Rendering.hpp"
#	include "Rendering/Sprite.hpp"
#
#	include "System/ResourceManager.hpp"
#
#	include "Rendering/UI/UIText.hpp"
#	include "Rendering/UI/UIGroup.hpp"
#	include "Rendering/UI/UIHorizontalGroup.hpp"
#	include "Rendering/UI/UIVerticalGroup.hpp"
#	include "Rendering/UI/UISprite.hpp"
#	include "Rendering/UI/UIButton.hpp"
#	include "Rendering/UI/UILayout.hpp"
#
#	include "Game/GameInterface.hpp"
#
#	include "Rendering/TiledMap.hpp"
#
#	include "Game/Console.hpp"
#
}
