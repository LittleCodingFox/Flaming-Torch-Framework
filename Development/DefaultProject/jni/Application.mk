NDK_TOOLCHAIN_VERSION := clang
APP_PLATFORM := android-18
APP_ABI := armeabi
APP_STL := c++_shared
APP_CFLAGS += -Wno-error=format-security
APP_MODULES := FlamingDependencies openal sndfile jpeg main
