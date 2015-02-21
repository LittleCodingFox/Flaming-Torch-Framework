#include "FlamingConfig.hpp"
#
#ifdef EMSCRIPTEN
#	include <emscripten.h>
#endif
#
#include "System/CoreVersion.hpp"
#include <vector>
#include <map>
#include <list>
#include <sstream>
#include <thread>
#include <mutex>
#include <algorithm>
#include "MaxRectsBinPack.h"
#
extern "C"
{
#	include <lua/lua.h>
#	include <lua/lauxlib.h>
#	include <lua/lualib.h>
}
#	include <json/json.h>
#	include <ft2build.h>
#	include FT_FREETYPE_H
#	if USE_GRAPHICS
#		include <tb/tb_renderer.h>
#		include <tb/tb_system.h>
#		include <tb/tb_bitmap_fragment.h>
#		include <tb/tb_skin.h>
#		include <tb/tb_widgets.h>
#		include <tb/tb_msg.h>
#		include <tb/tb_font_renderer.h>
#		include <tb/tb_widgets_reader.h>
#		include <tb/tb_widgets.h>
#		include <tb/tb_window.h>
#		include <tb/animation/tb_widget_animation.h>
#		include <tb/renderers/tb_renderer_batcher.h>
#
using namespace tb;
#
#	endif
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
#	include "System/ResourceManager.hpp"
#	include "Rendering/Rendering.hpp"
#
#	include "Game/GameInterface.hpp"
#
#	include "Rendering/Sprite.hpp"
#	include "Rendering/TiledMap.hpp"
#
#	include "Game/Console.hpp"
#
}
