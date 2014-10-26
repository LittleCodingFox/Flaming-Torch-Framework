#include <SFML/Config.hpp>

#ifdef SFML_SYSTEM_ANDROID

#include <SFML/System/Android/Activity.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Thread.hpp>
#include <SFML/System/Lock.hpp>
#include <SFML/System/Err.hpp>
#include <android/window.h>
#include <android/native_activity.h>
#include <android/log.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdlib.h>
#include <jni.h>
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_INFO, "AndroidBootstrap", __VA_ARGS__))

int main(int argc, char **argv);

extern "C" void AndroidBootstrap(sf::priv::ActivityStates *states, JavaVM *vm, JNIEnv *env)
{
    sf::priv::getActivity(states, true);
	LOGE("Main Loop starting!");

	int result = main(0, NULL);

	LOGE("Finished; Result: %d", result);
};

#endif

