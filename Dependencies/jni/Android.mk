LOCAL_PATH := $(call my-dir)
LOCAL_PATH_MINUS_ONE_LEVEL := $(LOCAL_PATH)/../

include $(CLEAR_VARS)

LOCAL_MODULE := FlamingDependencies
LOCAL_CPP_FEATURES += exceptions
LOCAL_CPP_FEATURES += rtti

GLOBAL_INCLUDES := $(LOCAL_PATH_MINUS_ONE_LEVEL)/Headers/ \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Headers/lua/ \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Headers/zlib/ \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/webp/ \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/ \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/Headers/AL/ \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/Headers/libsndfile/android/ \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/Headers/jpeg/

FREETYPE_SOURCES := $(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/autofit/autofit.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftsystem.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftinit.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftdebug.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftbase.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftbbox.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftglyph.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftbdf.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftbitmap.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftcid.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftfstype.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftgasp.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftgxval.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftlcdfil.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftmm.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftotval.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftpatent.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftpfr.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftstroke.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftsynth.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/fttype1.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftwinfnt.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/base/ftxf86.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/bdf/bdf.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/bzip2/ftbzip2.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/cache/ftcache.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/cff/cff.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/cid/type1cid.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/gxvalid/gxvalid.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/gzip/ftgzip.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/lzw/ftlzw.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/otvalid/otvalid.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/pcf/pcf.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/pfr/pfr.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/psaux/psaux.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/pshinter/pshinter.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/psnames/psnames.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/raster/ftraster.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/sfnt/sfnt.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/smooth/smooth.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/truetype/truetype.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/type1/type1.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/type42/type42.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/freetype/winfonts/winfnt.c

BASE64_SOURCES := $(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/base64/*.cpp)

ENET_SOURCES := $(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/enet/*.c)

LIBPNG_SOURCES := $(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/libpng/*.c)

JSONCPP_SOURCES := $(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/jsoncpp/*.cpp)

LUABIND_SOURCES := $(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/luabind/*.cpp)

TINYXML_SOURCES := $(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/tinyxml/*.cpp)

WEBP_SOURCES := $(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/webp/*.cpp)

ZLIB_SOURCES := $(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/zlib/*.cpp)

LUA_SOURCES := $(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/lapi.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/lauxlib.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/lbaselib.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/lcode.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/ldblib.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/ldebug.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/ldo.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/ldump.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/lfunc.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/lgc.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/linit.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/liolib.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/llex.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/lmathlib.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/lmem.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/loadlib.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/lobject.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/lopcodes.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/loslib.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/lparser.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/lstate.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/lstring.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/lstrlib.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/ltable.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/ltablib.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/ltm.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/lundump.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/lvm.c \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/lua/lzio.c

ROOT_SOURCES := $(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/*.cpp)

SFML_SOURCES := $(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/System/*.cpp) \
				$(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/System/Android/*.cpp) \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Window/Context.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Window/EGLCheck.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Window/EglContext.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Window/GlResource.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Window/Joystick.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Window/JoystickManager.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Window/Keyboard.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Window/Mouse.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Window/Sensor.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Window/SensorManager.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Window/Touch.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Window/VideoMode.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Window/Window.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Window/WindowImpl.cpp \
				$(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Window/Android/*.cpp) \
				$(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Network/*.cpp) \
				$(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Network/Unix/*.cpp) \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Main/MainAndroid.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Main/SFMLActivity.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/BlendMode.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/CircleShape.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/Color.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/ConvexShape.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/Font.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/Image.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/ImageLoader.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/RectangleShape.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/RenderStates.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/RenderTarget.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/RenderTexture.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/RenderTextureImpl.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/RenderTextureImplDefault.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/RenderTextureImplFBO.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/RenderWindow.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/Shader.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/Shape.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/Sprite.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/Text.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/Texture.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/TextureSaver.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/Transform.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/Transformable.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/Vertex.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/VertexArray.cpp \
				$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Graphics/View.cpp \
				$(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/SFML/SFML/Audio/*.cpp)

LOCAL_CFLAGS := -DGL_GLEXT_PROTOTYPES -D__ANDROID__ -D__unix__ -DSFML_OPENGL_ES -DSFML_WINDOW_EXPORTS -DSFML_SYSTEM_EXPORTS -DSFML_NETWORK_EXPORTS -DSFML_GRAPHICS_EXPORTS -DSFML_AUDIO_EXPORTS -DGLEW_STATIC -DUNICODE -DFT2_BUILD_LIBRARY
LOCAL_C_INCLUDES := $(GLOBAL_INCLUDES)

LOCAL_SRC_FILES := $(LUA_SOURCES) $(SFML_SOURCES) $(BASE64_SOURCES) $(FREETYPE_SOURCES) $(LIBPNG_SOURCES) $(ENET_SOURCES) $(JSONCPP_SOURCES)\
	$(LUABIND_SOURCES) $(TINYXML_SOURCES) $(WEBP_SOURCES) $(ZLIB_SOURCES) $(ROOT_SOURCES)

LOCAL_SHORT_COMMANDS := true
include $(BUILD_STATIC_LIBRARY)
