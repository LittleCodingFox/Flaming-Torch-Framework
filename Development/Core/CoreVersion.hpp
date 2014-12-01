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
#include <SimpleDelegate/SimpleDelegate.hpp>

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
#define OBJECTMODEL_PRIORITY 8
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
		friend class DisposablePointer;
	private:
		type *ContainedObject;
		bool WeakReference;

		DisposableResource<type>(const DisposableResource<type> &);
		DisposableResource<type> &operator=(const DisposableResource<type> &);
	public:

		DisposableResource<type>() : ContainedObject(NULL), WeakReference(false)
		{
		};

		explicit DisposableResource<type>(type *Object) : ContainedObject(Object), WeakReference(false)
		{
		};

		~DisposableResource<type>()
		{
			Dispose();
		};

		inline void MakeWeakReference()
		{
			WeakReference = true;
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
			if (ContainedObject)
			{
				if (!WeakReference)
				{
					type *ActualContainedObject = ContainedObject;

					ContainedObject = NULL;

					delete ActualContainedObject;
				}
				else
				{
					return;
				};
			};
		};
	};

	template<typename type>
	class DisposablePointer : private std::shared_ptr<DisposableResource<type> >
	{
	private:
		bool ReadOnly;
	public:
		DisposablePointer() : ReadOnly(false)
		{
		};

		explicit DisposablePointer(type *In) : ReadOnly(false)
		{
			this->reset(new DisposableResource<type>(In));
		};

		DisposablePointer(const DisposablePointer<type> &o) : std::shared_ptr<DisposableResource<type> >(o), ReadOnly(o.ReadOnly)
		{
		};

		inline bool operator==(const DisposablePointer<type> &o)
		{
			return o.Get() == Get();
		};

		inline bool operator!=(const DisposablePointer<type> &o)
		{
			return o.Get() != Get();
		};

		inline DisposablePointer<type> &operator=(const DisposablePointer<type> &o)
		{
			std::shared_ptr<DisposableResource<type> >::operator=(*static_cast<const std::shared_ptr<DisposableResource<type> > *>(&o));

			ReadOnly = o.ReadOnly;

			return *this;
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

		inline bool IsReadOnly()
		{
			return ReadOnly;
		};

		inline void MakeReadOnly()
		{
			ReadOnly = true;
		};

		inline void Dispose()
		{
			if(!ReadOnly && this->get())
			{
				this->get()->Dispose();
			};
		};

		inline void Reset(type *New)
		{
			if(ReadOnly)
				return;

			this->reset(new DisposableResource<type>(New));
		};

		inline uint32 UseCount() const
		{
			return this->use_count();
		};

		template<typename OutType>
		operator DisposablePointer<OutType>()
		{
			if(!Get())
				return DisposablePointer<OutType>();

#if CHECK_SSP_CONVERSIONS
			if(dynamic_cast<OutType *>(Get()) == NULL)
				return DisposablePointer<OutType>();
#endif

			return *(DisposablePointer<OutType> *)this;
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

		static DisposablePointer<type> MakeWeak(type *In)
		{
			DisposablePointer<type> Out(In);
			Out.get()->MakeWeakReference();

			return Out;
		};
	};

	/*!
	*	Core Utilities
	*/
	class CoreUtils
	{
	public:
		/*!
		*	\return a string with the name of this platform
		*	\param IncludeVersion whether to include version info
		*/
		static std::string PlatformString(bool IncludeVersion = false);

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
