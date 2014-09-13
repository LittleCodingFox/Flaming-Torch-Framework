#include "FlamingCore.hpp"

#ifdef FLPLATFORM_WINDOWS
#	define WIN32_LEAN_AND_MEAN
#	define VC_LEAN_AND_MEAN
#	include <windows.h>
#else
#	include <dlfcn.h>
#	include <unistd.h>
#endif
namespace FlamingTorch
{
	DynamicLibrary::DynamicLibrary()
	{
		Handle = NULL;
	};

	DynamicLibrary::~DynamicLibrary()
	{
		if(Handle)
		{
#ifdef FLPLATFORM_WINDOWS
			FreeLibrary((HMODULE)Handle);
#else
			dlclose(Handle);
#endif
		};
	};

	bool DynamicLibrary::Load(const std::string &FileName)
	{
		if(Handle)
		{
#ifdef FLPLATFORM_WINDOWS
			FreeLibrary((HMODULE)Handle);
#else
			dlclose(Handle);
#endif
		};

#ifdef FLPLATFORM_WINDOWS
		Handle = LoadLibraryA(FileName.c_str());
#else
		Handle = dlopen(FileName.c_str(), RTLD_NOW | RTLD_GLOBAL );
#endif

		return Handle != NULL;
	};

	void* DynamicLibrary::GetAddressAt(const std::string &ProcName)
	{
		SFLASSERT(Handle && ProcName.length());

#ifdef FLPLATFORM_WINDOWS
		return (void*)GetProcAddress((HMODULE)Handle, ProcName.c_str());
#else
		return dlsym(Handle, ProcName.c_str());
#endif
	};

};
