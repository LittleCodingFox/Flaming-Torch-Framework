#pragma once

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
	}

	explicit DisposableResource<type>(type *Object) : ContainedObject(Object), WeakReference(false)
	{
	}

	~DisposableResource<type>()
	{
		Dispose();
	}

	inline void MakeWeakReference()
	{
		WeakReference = true;
	}

	inline type *Get()
	{
		return ContainedObject;
	}

	inline const type *Get() const
	{
		return ContainedObject;
	}

	inline type *operator->()
	{
		return ContainedObject;
	}

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
			}
		}
	}
};

template<typename type>
class DisposablePointer : private std::shared_ptr < DisposableResource<type> >
{
private:
	bool ReadOnly;
public:
	DisposablePointer() : ReadOnly(false)
	{
	}

	explicit DisposablePointer(type *In) : ReadOnly(false)
	{
		this->reset(new DisposableResource<type>(In));
	}

	DisposablePointer(const DisposablePointer<type> &o) : std::shared_ptr<DisposableResource<type> >(o), ReadOnly(o.ReadOnly)
	{
	}

	inline bool operator==(const DisposablePointer<type> &o)
	{
		return o.Get() == Get();
	}

	inline bool operator!=(const DisposablePointer<type> &o)
	{
		return o.Get() != Get();
	}

	inline DisposablePointer<type> &operator=(const DisposablePointer<type> &o)
	{
		std::shared_ptr<DisposableResource<type> >::operator=(*static_cast<const std::shared_ptr<DisposableResource<type> > *>(&o));

		ReadOnly = o.ReadOnly;

		return *this;
	}

	inline type *Get()
	{
		return this->get() != NULL ? this->get()->Get() : NULL;
	}

	inline const type *Get() const
	{
		return this->get() != NULL ? this->get()->Get() : NULL;
	}

	inline type *operator->()
	{
		return Get();
	}

	inline const type *operator->() const
	{
		return Get();
	}

	inline operator type*()
	{
		return Get();
	}

	inline operator const type*() const
	{
		return Get();
	}

	inline const type& operator*() const
	{
		return *Get();
	}

	inline type& operator*()
	{
		return *Get();
	}

	inline operator bool() const
	{
		return Get() != NULL;
	}

	inline operator bool()
	{
		return Get() != NULL;
	}

	inline bool IsReadOnly()
	{
		return ReadOnly;
	}

	inline void MakeReadOnly()
	{
		ReadOnly = true;
	}

	inline void Dispose()
	{
		if (!ReadOnly && this->get())
		{
			this->get()->Dispose();
		}
	}

	inline void Reset(type *New)
	{
		if (ReadOnly)
			return;

		this->reset(new DisposableResource<type>(New));
	}

	inline uint32 UseCount() const
	{
		return this->use_count();
	}

	template<typename OutType>
	operator DisposablePointer<OutType>()
	{
		if (!Get())
			return DisposablePointer<OutType>();

#if CHECK_SSP_CONVERSIONS
		if (dynamic_cast<OutType *>(Get()) == NULL)
			return DisposablePointer<OutType>();
#endif

		return *(DisposablePointer<OutType> *)this;
	}

	template<typename OutType>
	inline OutType *AsDerived()
	{
#if CHECK_SSP_CONVERSIONS
		return dynamic_cast<OutType *>(Get());
#else
		return static_cast<OutType *>(Get());
#endif
	}

	static DisposablePointer<type> MakeWeak(type *In)
	{
		DisposablePointer<type> Out(In);
		Out.get()->MakeWeakReference();

		return Out;
	}
};
