#include <prototype/prototype.h>
#include <prototype/window.h>
#include <prototype/driver.h>
#include <netlib/netlib.h>
#include <netlib/thread.h>

using namespace netlib;

namespace prototype
{
	PROTOTYPE_API bool init()
	{
		if(!netlib::init())
			return false;

		if(!window::init())
			return false;

		if(!driver::register_static_drivers())
			return false;

		return true;
	}
}