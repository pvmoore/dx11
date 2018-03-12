#pragma once

#if NDEBUG
#define _RELEASE
#else
#undef _RELEASE
#endif

typedef signed char sbyte;
typedef unsigned char ubyte;
typedef unsigned int uint;
typedef unsigned long long ulong;
typedef signed long long slong;

#pragma warning(disable : 4101) /// unused variable
