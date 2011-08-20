#include <prototype/driver.h>
#include <prototype/driver_internal.h>
#include <netlib/thread.h>

using namespace netlib;

namespace prototype
{
	driver::drivers_t driver::gDrivers;
	static critical_section gDriversCS;

	driver::driver(std::string const& _nm)
		: mName(_nm)
	{
	}

	bool driver::register_driver()
	{
		if(mRegistered)
			return false;

		gDriversCS.lock();
		if(gDrivers.find(mName) != gDrivers.end())
		{
			gDriversCS.unlock();
			return false;
		}

		gDrivers.insert(drivers_t::value_type(mName, this));
		gDriversCS.unlock();

		init();
		return true;
	}

	void driver::unregister_driver()
	{
		if(!mRegistered)
			return;

		gDriversCS.lock();
		auto it = gDrivers.find(mName);
		if(it != gDrivers.end())
			gDrivers.erase(it);
		gDriversCS.unlock();

		// Shutdown called automatically
		// when all handles released.
	}

	void driver::destroy()
	{
		if(mInitDone)
			shutdown();
		delete this;
	}

	bool driver::init()
	{
		if(mInitDone)
			return false;

		mInitDone = true;
		return true;
	}

	void driver::shutdown()
	{
		mInitDone = false;
	}
	
	bool driver::register_static_drivers()
	{
		bool success = true;

		for(auto it = static_driver_registration::static_drivers().begin();
			it != static_driver_registration::static_drivers().end(); it++)
			success &= (*it)->register_driver();

		return success;
	}
}
