#include <prototype/input.h>
#include <prototype/binding.h>
#include <algorithm>
#include <stdexcept>
#include <functional>

using namespace std::placeholders;

namespace prototype
{
	//
	// input_control
	//
	
	input_control::input_control(handle<input_device> const& _dev, std::string const& _name)
		: mName(_name), mDevice(_dev)
	{
	}

	//
	// input_device
	//

	input_device::input_device(handle<input_driver> const& _dev, std::string const& _nm)
		: mName(_nm), mDriver(_dev)
	{
		acquire();
	}

	bool input_device::add_control(handle<input_control> const& _ctl)
	{
		auto it = mControlMap.find(_ctl->name());
		if(it != mControlMap.end())
			return false;

		mControlMap.insert(control_map_t::value_type(_ctl->name(), _ctl));
		mControls.push_back(_ctl);
		return true;
	}

	void input_device::remove_control(handle<input_control> const& _ctl)
	{
		auto it = mControlMap.find(_ctl->name());
		if(it == mControlMap.end())
			return;
		
		mControlMap.erase(it);
		mControls.erase(std::find(mControls.begin(), mControls.end(), _ctl));
	}
	
	//
	// input_driver
	//

	input_driver::drivers_t input_driver::gDrivers;
	delegate<input::driver_added_event> input_driver::gDriverAdded;
	delegate<input::driver_removed_event> input_driver::gDriverRemoved;

	input_driver::input_driver(std::string const& _nm)
		: driver(_nm)
	{
	}

	void input_driver::enumerate()
	{
	}

	bool input_driver::add_device(handle<input_device> const& _ctl)
	{
		auto it = mDevices.find(_ctl);
		if(it != mDevices.end())
			return false;

		mDevices.insert(_ctl);
		input::device_added_event evt(this, _ctl.get());
		mDeviceAdded(evt);
		return true;
	}

	void input_driver::remove_device(handle<input_device> const& _ctl)
	{
		auto it = mDevices.find(_ctl);
		if(it == mDevices.end())
			return;

		input::device_removed_event evt(this, _ctl.get());
		mDeviceRemoved(evt);
		mDevices.erase(it);
	}

	bool input_driver::init()
	{
		if(!driver::init())
			return false;

		input::driver_added_event evt(this);
		gDriverAdded(evt);
		return true;
	}

	void input_driver::shutdown()
	{
		input::driver_removed_event evt(this);
		gDriverRemoved(evt);

		driver::shutdown();
	}

	//
	// input_controller
	//
	
	input_controller::input_controller()
	{
		mVAHandler = input_driver::on_driver_added() += std::bind(&input_controller::driver_added, this, _1);
		mVRHandler = input_driver::on_driver_removed() += std::bind(&input_controller::driver_removed, this, _1);

		std::for_each(input_driver::drivers().begin(), input_driver::drivers().end(),
			[this](handle<input_driver> const& _drv) {
				pair_t vals;
				vals.first = _drv->on_device_added() += std::bind(&input_controller::device_added, this, _1);
				vals.second = _drv->on_device_removed() += std::bind(&input_controller::device_added, this, _1);

				mDHandler.insert(input_controller::device_map_t::value_type(_drv, vals));
		});
	}

	input_controller::~input_controller()
	{
		std::for_each(input_driver::drivers().begin(), input_driver::drivers().end(),
			[this](handle<input_driver> const& _drv) {
				auto it = mDHandler.find(_drv);
				if(it != mDHandler.end())
				{
					_drv->on_device_added() -= it->second.first;
					_drv->on_device_removed() -= it->second.second;
				}
		});

		input_driver::on_driver_added() -= mVAHandler;
		input_driver::on_driver_removed() -= mVRHandler;
	}
		
	bool input_controller::driver_added(input::driver_added_event const& _evt)
	{
		pair_t vals;
		input_driver *drv = _evt.driver();
		vals.first = drv->on_device_added() += std::bind(&input_controller::device_added, this, _1);
		vals.second = drv->on_device_removed() += std::bind(&input_controller::device_removed, this, _1);
		mDHandler.insert(input_controller::device_map_t::value_type(drv, vals));
		return false;
	}

	bool input_controller::driver_removed(input::driver_removed_event const& _evt)
	{
		input_driver *drv = _evt.driver();
		auto it = mDHandler.find(drv);
		if(it != mDHandler.end())
		{
			drv->on_device_added() -= it->second.first;
			drv->on_device_removed() -= it->second.second;
		}
		return false;
	}

	bool input_controller::device_added(input::device_added_event const&_evt)
	{
		mDeviceAdded(_evt);
		return false;
	}

	bool input_controller::device_removed(input::device_removed_event const&_evt)
	{
		mDeviceRemoved(_evt);
		return false;
	}

	namespace input
	{

		//
		// events
		//

		device_added_event::device_added_event(input_driver *_drv, input_device *_dev)
			: event(_dev), mDevice(_dev)
		{
		}

		driver_added_event::driver_added_event(input_driver *_drv)
			: event(_drv)
		{
		}
	}
}
