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

	using netlib::shutdown;
	using netlib::running;
	using netlib::exit;
	using netlib::exit_value;
	using netlib::think;
	using netlib::run_main_loop;
}