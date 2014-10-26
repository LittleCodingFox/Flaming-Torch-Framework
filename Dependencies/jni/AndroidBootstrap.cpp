#include <SFML/Config.hpp>

#ifdef SFML_SYSTEM_ANDROID

#include <SFML/System/Android/Activity.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Thread.hpp>
#include <SFML/System/Lock.hpp>
#include <SFML/System/Err.hpp>
#include <android/window.h>
#include <android/native_activity.h>
#include <android/native_activity.h>
#include <android/log.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdlib.h>
#include <jni.h>
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_INFO, "AndroidBootstrap", __VA_ARGS__))
#define MAINNAME "main"

typedef void (*StartupProc)(sf::priv::ActivityStates *states, JavaVM *vm, JNIEnv *env);

void* loadLibrary(const char* libraryName, JNIEnv* lJNIEnv, jobject& ObjectActivityInfo)
{
    // Find out the absolute path of the library
    jclass ClassActivityInfo = lJNIEnv->FindClass("android/content/pm/ActivityInfo");
    jfieldID FieldApplicationInfo = lJNIEnv->GetFieldID(ClassActivityInfo, "applicationInfo", "Landroid/content/pm/ApplicationInfo;");
    jobject ObjectApplicationInfo = lJNIEnv->GetObjectField(ObjectActivityInfo, FieldApplicationInfo);

    jclass ClassApplicationInfo = lJNIEnv->FindClass("android/content/pm/ApplicationInfo");
    jfieldID FieldNativeLibraryDir = lJNIEnv->GetFieldID(ClassApplicationInfo, "nativeLibraryDir", "Ljava/lang/String;");

    jobject ObjectDirPath = lJNIEnv->GetObjectField(ObjectApplicationInfo, FieldNativeLibraryDir);

    jclass ClassSystem = lJNIEnv->FindClass("java/lang/System");
    jmethodID StaticMethodMapLibraryName = lJNIEnv->GetStaticMethodID(ClassSystem, "mapLibraryName", "(Ljava/lang/String;)Ljava/lang/String;");

    jstring LibNameObject = lJNIEnv->NewStringUTF(libraryName);
    jobject ObjectName = lJNIEnv->CallStaticObjectMethod(ClassSystem, StaticMethodMapLibraryName, LibNameObject);

    jclass ClassFile = lJNIEnv->FindClass("java/io/File");
    jmethodID FileConstructor = lJNIEnv->GetMethodID(ClassFile, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
    jobject ObjectFile = lJNIEnv->NewObject(ClassFile, FileConstructor, ObjectDirPath, ObjectName);

    // Get the library absolute path and convert it
    jmethodID MethodGetPath = lJNIEnv->GetMethodID(ClassFile, "getPath", "()Ljava/lang/String;");
    jstring javaLibraryPath = static_cast<jstring>(lJNIEnv->CallObjectMethod(ObjectFile, MethodGetPath));
    const char* libraryPath = lJNIEnv->GetStringUTFChars(javaLibraryPath, NULL);

    // Manually load the library
    void * handle = dlopen(libraryPath, RTLD_NOW | RTLD_GLOBAL);
    if (!handle)
    {
        LOGE("dlopen(\"%s\"): %s", libraryPath, dlerror());
        exit(1);
    }

    // Release the Java string
    lJNIEnv->ReleaseStringUTFChars(javaLibraryPath, libraryPath);

    return handle;
}

void Bootstrap(sf::priv::ActivityStates *states, JavaVM *vm, JNIEnv *lJNIEnv)
{
	LOGE("Bootstrapping!");

    // Retrieve the NativeActivity
    jobject ObjectNativeActivity = states->activity->clazz;
    jclass ClassNativeActivity = lJNIEnv->GetObjectClass(ObjectNativeActivity);

    // Retrieve the ActivityInfo
    jmethodID MethodGetPackageManager = lJNIEnv->GetMethodID(ClassNativeActivity, "getPackageManager", "()Landroid/content/pm/PackageManager;");
    jobject ObjectPackageManager = lJNIEnv->CallObjectMethod(ObjectNativeActivity, MethodGetPackageManager);

    jmethodID MethodGetIndent = lJNIEnv->GetMethodID(ClassNativeActivity, "getIntent", "()Landroid/content/Intent;");
    jobject ObjectIntent = lJNIEnv->CallObjectMethod(ObjectNativeActivity, MethodGetIndent);

    jclass ClassIntent = lJNIEnv->FindClass("android/content/Intent");
    jmethodID MethodGetComponent = lJNIEnv->GetMethodID(ClassIntent, "getComponent", "()Landroid/content/ComponentName;");

    jobject ObjectComponentName = lJNIEnv->CallObjectMethod(ObjectIntent, MethodGetComponent);

    jclass ClassPackageManager = lJNIEnv->FindClass("android/content/pm/PackageManager");

    jfieldID FieldGET_META_DATA = lJNIEnv->GetStaticFieldID(ClassPackageManager, "GET_META_DATA", "I");
    jint GET_META_DATA = lJNIEnv->GetStaticIntField(ClassPackageManager, FieldGET_META_DATA);

    jmethodID MethodGetActivityInfo = lJNIEnv->GetMethodID(ClassPackageManager, "getActivityInfo", "(Landroid/content/ComponentName;I)Landroid/content/pm/ActivityInfo;");
    jobject ObjectActivityInfo = lJNIEnv->CallObjectMethod(ObjectPackageManager, MethodGetActivityInfo, ObjectComponentName, GET_META_DATA);

	LOGE("Loading Bootstrap Main Library '%s'", MAINNAME);

	void *Handle = loadLibrary(MAINNAME, lJNIEnv, ObjectActivityInfo);

	if(!Handle)
	{
		LOGE("Cannot load '%s'!", MAINNAME);

		exit(1);
	};

	StartupProc Startup = (StartupProc)dlsym(Handle, "AndroidBootstrap");

	if(!Startup)
	{
        LOGE("Undefined symbol AndroidBootstrap");
		
		exit(1);
	};

	LOGE("Starting up!");

	Startup(states, vm, lJNIEnv);
}

#endif // SFML_SYSTEM_ANDROID

