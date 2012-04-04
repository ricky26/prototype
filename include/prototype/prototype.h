#include <stdint.h>
#include <stddef.h>
#include <netlib/netlib.h>

#pragma once

#ifdef PROTOTYPE_EXPORTS
#	define PROTOTYPE_API NETLIB_API_EXPORT
#else
#	define PROTOTYPE_API NETLIB_API_IMPORT
#endif

#define PROTOTYPE_THREAD	NETLIB_THREAD
#define PROTOTYPE_INLINE	NETLIB_INLINE

namespace prototype
{
	PROTOTYPE_API bool init();

	using namespace netlib;
}