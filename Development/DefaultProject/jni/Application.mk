NDK_TOOLCHAIN_VERSION := clang
APP_PLATFORM := android-18
APP_ABI := all32
APP_STL := c++_shared
APP_OPTIM := release
APP_CFLAGS += -Wno-error=format-security
APP_MODULES := FlamingDependencies openal sndfile jpeg main
