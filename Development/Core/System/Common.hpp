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
