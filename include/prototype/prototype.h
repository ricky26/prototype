#pragma once

#ifdef _MSC_VER
#	ifdef PROTOTYPE_EXPORTS
#		define PROTOTYPE_API __declspec(dllexport)
#	else
#		define PROTOTYPE_API __declspec(dllimport)
#	endif
#	define PROTOTYPE_INLINE __forceinline
#	define PROTOTYPE_THREAD	__declspec(thread)

	// Disable some stupid MSVC warnings
#	pragma warning(disable:4251 4996 4355)
#else
#	ifdef PROTOTYPE_EXPORTS
#		define PROTOTYPE_API 
#	else
#		define PROTOTYPE_API 
#	endif
#	define PROTOTYPE_INLINE inline
#	define PROTOTYPE_THRED __thread
#endif

#include <stdint.h>
#include <stddef.h>
#include <netlib/netlib.h>

namespace prototype
{
	PROTOTYPE_API bool init();
	PROTOTYPE_API void shutdown();
	PROTOTYPE_API void exit(int _val=0);
	PROTOTYPE_API int exit_value();
	PROTOTYPE_API bool think();
	PROTOTYPE_API int run_main_loop();

	using netlib::running;

	PROTOTYPE_API uint64_t time();
}