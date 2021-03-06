LOCAL_PATH := $(call my-dir)
LOCAL_PATH_MINUS_ONE_LEVEL := $(LOCAL_PATH)/../
LOCAL_PATH_DEPENDENCIES_FOLDER := $(LOCAL_PATH_MINUS_ONE_LEVEL)/../../Dependencies/

include $(CLEAR_VARS)

LOCAL_MODULE    := FlamingDependencies
LOCAL_SRC_FILES := $(LOCAL_PATH_DEPENDENCIES_FOLDER)/obj/local/$(TARGET_ARCH_ABI)/libFlamingDependencies.a

include $(PREBUILT_STATIC_LIBRARY)

ifeq ($(TARGET_ARCH_ABI),$(filter $(TARGET_ARCH_ABI), armeabi-v7a x86))

include $(CLEAR_VARS)

LOCAL_MODULE    := webp-neon
LOCAL_SRC_FILES := $(LOCAL_PATH_DEPENDENCIES_FOLDER)/obj/local/$(TARGET_ARCH_ABI)/libwebp-neon.a

include $(PREBUILT_STATIC_LIBRARY)

endif # TARGET_ARCH_ABI == armeabi-v7a || x86

include $(CLEAR_VARS)

LOCAL_MODULE := sndfile
LOCAL_SRC_FILES := $(LOCAL_PATH_DEPENDENCIES_FOLDER)/Libs/Android/$(TARGET_ARCH_ABI)/libsndfile.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := jpeg
LOCAL_SRC_FILES := $(LOCAL_PATH_DEPENDENCIES_FOLDER)/Libs/Android/$(TARGET_ARCH_ABI)/libjpeg.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := openal
LOCAL_SRC_FILES := $(LOCAL_PATH_DEPENDENCIES_FOLDER)/Libs/Android/$(TARGET_ARCH_ABI)/libopenal.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := main
LOCAL_CPP_FEATURES += exceptions
LOCAL_CPP_FEATURES += rtti

GLOBAL_INCLUDES := $(LOCAL_PATH_DEPENDENCIES_FOLDER)/Headers/ \
			$(LOCAL_PATH_DEPENDENCIES_FOLDER)/Headers/lua/ \
			$(LOCAL_PATH_DEPENDENCIES_FOLDER)/Headers/zlib/ \
			$(LOCAL_PATH_DEPENDENCIES_FOLDER)/Headers/zlib/ \
			$(LOCAL_PATH_DEPENDENCIES_FOLDER)/Source/webp/ \
			$(LOCAL_PATH_DEPENDENCIES_FOLDER)/Source/SFML/ \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/ \
			$(LOCAL_PATH_MINUS_ONE_LEVEL)/../Core/

CORE_SOURCES := $(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/../Core/*.cpp)\
		$(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/../Core/Audio/*.cpp)\
		$(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/../Core/Audio/Implementations/*.cpp)\
		$(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/../Core/Crossplatform/Android/*.cpp)\
		$(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/../Core/Game/*.cpp)\
		$(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/../Core/Rendering/*.cpp)\
		$(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/../Core/Rendering/Implementations/*.cpp)\
		$(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/../Core/Rendering/UI/*.cpp)\
		$(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/../Core/Scripting/*.cpp)\
		$(wildcard $(LOCAL_PATH_MINUS_ONE_LEVEL)/../Core/System/*.cpp)

LOCAL_CFLAGS := -DGL_GLEXT_PROTOTYPES -D__ANDROID__ -D__unix__ -DSFML_OPENGL_ES -DSFML_WINDOW_EXPORTS -DSFML_SYSTEM_EXPORTS -DSFML_NETWORK_EXPORTS -DSFML_GRAPHICS_EXPORTS -DSFML_AUDIO_EXPORTS -DGLEW_STATIC -DUNICODE -DFT2_BUILD_LIBRARY
LOCAL_CPP_FLAGS := -std=c++0x
LOCAL_C_INCLUDES := $(GLOBAL_INCLUDES)
LOCAL_LDLIBS := -lEGL -llog -landroid -lGLESv1_CM -ldl
LOCAL_STATIC_LIBRARIES := FlamingDependencies jpeg cpufeatures
LOCAL_SHARED_LIBRARIES := openal sndfile

ifeq ($(TARGET_ARCH_ABI),$(filter $(TARGET_ARCH_ABI), armeabi-v7a x86))

LOCAL_STATIC_LIBRARIES += webp-neon

endif # TARGET_ARCH_ABI == armeabi-v7a || x86

LOCAL_SRC_FILES := $(CORE_SOURCES) \
					$(LOCAL_PATH_MINUS_ONE_LEVEL)/Source/main.cpp

#LOCAL_SHORT_COMMANDS := true
include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/cpufeatures)

