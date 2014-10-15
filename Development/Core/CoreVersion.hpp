#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>
#include "Platform.hpp"
#include "Common.hpp"
#include <string.h>
#include <string>
#include <memory>
#include <map>
#include <math.h>
#include "Signal.h"
using namespace Gallant;

#define FTSTD_VERSION_MAJOR 0
#define FTSTD_VERSION_MINOR 11
#define FLAMINGRENDER_VERSION_ID 'fr01'

//Subsystem Priorities
#define LOG_PRIORITY 1
#define GAMECLOCK_PRIORITY 2
#define FPSCOUNTER_PRIORITY 3
#define LUASCRIPTMANAGER_PRIORITY 4
#define CONSOLE_PRIORITY 5
#define RENDERERMANAGER_PRIORITY 6
#define RESOURCEMANAGER_PRIORITY 7
#define NETWORK_PRIORITY 14
#define SOUNDMANAGER_PRIORITY 15
#define FILESYSTEM_WATCHER_PRIORITY 60
#define PACKAGE_FILESYSTEM_PRIORITY 61
#define PROFILER_PRIORITY 62
#define FUTURE_PRIORITY 63

namespace FlamingTorch
{
	typedef uint32 VersionType;
	class Rect;

	template<typename type>
	class DisposableResource
	{
		template<typename othertype>
		friend class SuperSmartPointer;
	private:
		type *ContainedObject;

		DisposableResource<type>(const DisposableResource<type> &);
		DisposableResource<type> &operator=(const DisposableResource<type> &);
	public:

		DisposableResource<type>() : ContainedObject(NULL)
		{
		};

		explicit DisposableResource<type>(type *Object) : ContainedObject(Object)
		{
		};

		~DisposableResource<type>()
		{
			Dispose();
		};

		inline type *Get()
		{
			return ContainedObject;
		};

		inline const type *Get() const
		{
			return ContainedObject;
		};

		inline type *operator->()
		{
			return ContainedObject;
		};

		inline void Dispose()
		{
			if(ContainedObject)
			{
				type *ActualContainedObject = ContainedObject;

				ContainedObject = NULL;

				delete ActualContainedObject;
			};
		};
	};

	template<typename type>
	class SuperSmartPointer : public std::shared_ptr<DisposableResource<type> >
	{
	public:
		SuperSmartPointer()
		{
		};

		explicit SuperSmartPointer(type *In)
		{
			reset(new DisposableResource<type>(In));
		};

		inline type *Get()
		{
			return this->get() != NULL ? this->get()->Get() : NULL;
		};

		inline const type *Get() const
		{
			return this->get() != NULL ? this->get()->Get() : NULL;
		};

		inline type *operator->()
		{
			return Get();
		};

		inline const type *operator->() const
		{
			return Get();
		};

		inline operator type*()
		{
			return Get();
		};

		inline operator const type*() const
		{
			return Get();
		};

		inline const type& operator*() const
		{
			return *Get();
		};

		inline type& operator*()
		{
			return *Get();
		};

		inline operator bool() const
		{
			return Get() != NULL;
		};

		inline operator bool()
		{
			return Get() != NULL;
		};

		inline void Dispose()
		{
			if(this->get())
			{
				this->get()->Dispose();
			};
		};

		inline void Reset(type *New)
		{
			this->reset(new DisposableResource<type>(New));
		};

		template<typename OutType>
		operator SuperSmartPointer<OutType>()
		{
			if(!Get())
				return SuperSmartPointer<OutType>();

			return *(SuperSmartPointer<OutType> *)this;
		};

		template<typename OutType>
		inline OutType *AsDerived()
		{
#if CHECK_SSP_CONVERSIONS
			return dynamic_cast<OutType *>(Get());
#else
			return static_cast<OutType *>(Get());
#endif
		};
	};

	/*!
	*	Core Utilities
	*/
	class CoreUtils
	{
	public:
		/*!
		*	Runs a program from the OS
		*	\param ExePath the path to the executable file
		*	\param Parameters the parameters to pass to the executable
		*	\param WorkingDirectory the directory from which to run the executable
		*	\return the return code from running the .exe, or -1 if there was a problem running it
		*	\note on non-Windows systems this always returns 0 for now
		*/
		static int32 RunProgram(const std::string &ExePath, const std::string &Parameters, const std::string &WorkingDirectory);

		/*!
		*	Makes a VersionType from two bytes
		*	\param a the major version
		*	\param b the minor version
		*/
		static inline VersionType MakeVersion(uint8 a, uint8 b)
		{
			VersionType Result = 0;
			uint8 *t = (uint8*)&Result;
			t[0] = a;
			t[1] = b;

			return Result;
		};

		/*!
		*	Makes a version string from two bytes
		*	\param a the major version
		*	\param b the minor version
		*/
		static std::string MakeVersionString(uint8 a, uint8 b);

		/*!
		*	Makes an Int from Bytes
		*	\param a the first byte
		*	\param b the second byte
		*	\param c the third byte
		*	\param d the fourth byte
		*/
		static inline int32 MakeIntFromBytes(uint8 a, uint8 b, uint8 c, uint8 d)
		{
			int32 Result = 0;
			uint8 *t = (uint8*)&Result;
			t[0] = a;
			t[1] = b;
			t[2] = c;
			t[3] = d;

			return Result;
		};

		/*!
		*	Makes an Int from Bytes
		*	\param Bytes an array of uint8
		*	\param Length the amount of bytes (max 4)
		*/
		static inline int32 MakeIntFromBytes(uint8 *Bytes, uint32 Length = 4)
		{
			int32 Result = 0;
			uint8 *t = (uint8*)&Result;
			t[0] = Bytes[0];

			if(Length > 1)
			{
				t[1] = Bytes[1];

				if(Length > 2)
				{
					t[2] = Bytes[2];

					if(Length > 3)
					{
						t[3] = Bytes[3];
					};
				};
			};

			return Result;
		};

		/*!
		*	\return the current desktop work area (position and size)
		*/
		static Rect GetDesktopWorkArea();
	};
};
