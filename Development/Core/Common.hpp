#pragma once
#define SFLASSERT(var) \
	if(!(var))\
	{\
		printf("ASSERT FAILED: %s at %s:%s:%d\n", #var, __FILE__, __FUNCTION__, __LINE__);\
		return 0;\
	}

#define SVOIDFLASSERT(var) \
	if(!(var))\
	{\
		printf("ASSERT FAILED: %s at %s:%s:%d\n", #var, __FILE__, __FUNCTION__, __LINE__);\
		return;\
	}

#define SFUNCFLASSERT(var, func) \
	if(!(var))\
	{\
		printf("ASSERT FAILED: %s at %s:%s:%d\n", #var, __FILE__, __FUNCTION__, __LINE__);\
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
#include <stdint.h>

namespace FlamingTorch
{
	typedef uint8_t uint8;
	typedef int8_t int8;
	typedef uint16_t uint16;
	typedef int16_t int16;
	typedef uint32_t uint32;
	typedef int32_t int32;
	typedef uint64_t uint64;
	typedef int64_t int64;
	typedef float f32;
	typedef double f64;

	void LibWarning(unsigned int Line, const char* FileName, 
		const char* Function, const char* Variable, const char *Message, ...);
};
