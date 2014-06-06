#pragma once
#define SFLASSERT(var) \
	if(!(var))\
	{\
		return 0;\
	}

#define SVOIDFLASSERT(var) \
	if(!(var))\
	{\
		return;\
	}

#define SFUNCFLASSERT(var, func) \
	if(!(var))\
	{\
		func;\
	}

#define FLASSERT(var, ...) \
{\
	if(!(var))\
	{\
		FlamingTorch::LibWarning(__LINE__,__FILE__,__FUNCTION__,#var, __VA_ARGS__);\
	}\
}

#define FLAGVALUE(x) (1 << x)

namespace FlamingTorch
{
	typedef unsigned char uint8;
	typedef signed char int8;
	typedef unsigned short uint16;
	typedef signed short int16;
	typedef unsigned long uint32;
	typedef signed long int32;
	typedef unsigned long long uint64;
	typedef signed long long int64;
	typedef float f32;
	typedef double f64;

	void FLAMING_API LibWarning(unsigned int Line, const char* FileName, 
		const char* Function, const char* Variable, const char *Message, ...);
};
