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
	delegate input_driver::gDriverAdded;
	delegate input_driver::gDriverRemoved;

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
		mDeviceAdded(&evt);
		return true;
	}

	void input_driver::remove_device(handle<input_device> const& _ctl)
	{
		auto it = mDevices.find(_ctl);
		if(it == mDevices.end())
			return;

		input::device_removed_event evt(this, _ctl.get());
		mDeviceRemoved(&evt);
		mDevices.erase(it);
	}

	bool input_driver::init()
	{
		if(!driver::init())
			return false;

		input::driver_added_event evt(this);
		gDriverAdded(&evt);
		return true;
	}

	void input_driver::shutdown()
	{
		input::driver_removed_event evt(this);
		gDriverRemoved(&evt);

		driver::shutdown();
	}

	//
	// input_controller
	//
	
	input_controller::input_controller()
		: mDrvAHandler(std::bind(&input_controller::driver_added, this, _1)),
			mDrvRHandler(std::bind(&input_controller::driver_removed, this, _1)),
			mDAHandler(std::bind(&input_controller::device_added, this, _1)),
			mDRHandler(std::bind(&input_controller::device_removed, this, _1))
	{
		input_driver::on_driver_added() += &mDrvAHandler;
		input_driver::on_driver_removed() += &mDrvRHandler;

		std::for_each(input_driver::drivers().begin(), input_driver::drivers().end(),
			[this](handle<input_driver> const& _drv) {
				_drv->on_device_added() += &mDAHandler;
				_drv->on_device_removed() += &mDRHandler;
		});
	}

	input_controller::~input_controller()
	{
		std::for_each(input_driver::drivers().begin(), input_driver::drivers().end(),
			[this](handle<input_driver> const& _drv) {
				_drv->on_device_added() -= &mDAHandler;
				_drv->on_device_removed() -= &mDRHandler;
		});

		input_driver::on_driver_added() -= &mDrvAHandler;
		input_driver::on_driver_removed() -= &mDrvRHandler;
	}
		
	void input_controller::driver_added(event *_evt)
	{
		input::driver_added_event *dae
			= static_cast<input::driver_added_event*>(_evt);

		input_driver *drv = dae->driver();
		drv->on_device_added() += &mDAHandler;
		drv->on_device_removed() += &mDRHandler;
	}

	void input_controller::driver_removed(event *_evt)
	{
		input::driver_removed_event *dae
			= static_cast<input::driver_removed_event*>(_evt);

		input_driver *drv = dae->driver();
		drv->on_device_added() -= &mDAHandler;
		drv->on_device_removed() -= &mDRHandler;
	}

	void input_controller::device_added(event *_evt)
	{
		mDeviceAdded(_evt);
	}

	void input_controller::device_removed(event *_evt)
	{
		mDeviceRemoved(_evt);
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
