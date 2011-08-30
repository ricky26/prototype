#include "driver.h"
#include <netlib/event.h>
#include <string>
#include <list>
#include <unordered_map>
#include <unordered_set>

#pragma once

namespace prototype
{
	using netlib::delegate;
	using netlib::event;
	
	class input_driver;
	class input_device;

	//
	// events
	//

	namespace input
	{
		class PROTOTYPE_API device_added_event: public event
		{
		public:
			device_added_event(input_driver *_drv, input_device *_dev);
		
			PROTOTYPE_INLINE input_driver *driver() const { return static_cast<input_driver*>(sender()); }
			PROTOTYPE_INLINE input_device *device() const { return mDevice; }

		private:
			input_device *mDevice;
		};
		typedef device_added_event device_removed_event;

		class PROTOTYPE_API driver_added_event: public event
		{
		public:
			driver_added_event(input_driver *_drv);
		
			PROTOTYPE_INLINE input_driver *driver() const { return static_cast<input_driver*>(sender()); }
		};
		typedef driver_added_event driver_removed_event;
	}

	//
	// input
	//

	class input_device;
	class PROTOTYPE_API input_control: public ref_counted
	{
	public:
		input_control(handle<input_device> const& _dev, std::string const& _name);

		PROTOTYPE_INLINE handle<input_device> device() const { return mDevice; }
		PROTOTYPE_INLINE std::string const& name() const { return mName; }

	private:
		std::string mName;
		handle<input_device> mDevice;
	};

	class input_driver;
	class PROTOTYPE_API input_device: public ref_counted
	{
	public:
		typedef std::list<handle<input_control> > controls_t;
		typedef std::unordered_map<std::string, handle<input_control> > control_map_t;

		input_device(handle<input_driver> const& _drv, std::string const& _nm);
		
		PROTOTYPE_INLINE std::string const& name() const { return mName; }
		PROTOTYPE_INLINE handle<input_driver> driver() const { return mDriver; }
		PROTOTYPE_INLINE controls_t const& controls() const { return mControls; }

	protected:
		bool add_control(handle<input_control> const& _ctl);
		void remove_control(handle<input_control> const& _ctl);

	private:
		std::string mName;
		handle<input_driver> mDriver;
		controls_t mControls;
		control_map_t mControlMap;
	};

	/** Represents an input driver. IE, DirectInput/XInput/jsdev. */
	class PROTOTYPE_API input_driver: public driver
	{
	public:
		typedef std::unordered_set<handle<input_device> > devices_t;
		typedef std::unordered_set<handle<input_driver> > drivers_t;

		input_driver(std::string const& _nm);

		virtual void enumerate();

		PROTOTYPE_INLINE devices_t const& devices() const { return mDevices; }
		PROTOTYPE_INLINE delegate<input::device_added_event> &on_device_added() { return mDeviceAdded; }
		PROTOTYPE_INLINE delegate<input::device_removed_event> &on_device_removed() { return mDeviceRemoved; }

		static PROTOTYPE_INLINE drivers_t const& drivers() { return gDrivers; }
		static PROTOTYPE_INLINE delegate<input::driver_added_event> &on_driver_added() { return gDriverAdded; }
		static PROTOTYPE_INLINE delegate<input::driver_removed_event> &on_driver_removed() { return gDriverRemoved; }

	protected:
		bool add_device(handle<input_device> const& _ctl);
		void remove_device(handle<input_device> const& _ctl);

		bool init() override;
		void shutdown() override;

	private:
		devices_t mDevices;
		delegate<input::device_added_event> mDeviceAdded;
		delegate<input::device_removed_event> mDeviceRemoved;

		static drivers_t gDrivers;
		static delegate<input::driver_added_event> gDriverAdded;
		static delegate<input::driver_removed_event> gDriverRemoved;
	};

	class PROTOTYPE_API input_controller
	{
	public:
		input_controller();
		virtual ~input_controller();
		
		PROTOTYPE_INLINE delegate<input::device_added_event> &on_device_added() { return mDeviceAdded; }
		PROTOTYPE_INLINE delegate<input::device_removed_event> &on_device_removed() { return mDeviceRemoved; }

	protected:
		bool driver_added(input::driver_added_event const& _evt);
		bool driver_removed(input::driver_removed_event const& _evt);

		bool device_added(input::device_added_event const& _evt);
		bool device_removed(input::device_removed_event const& _evt);

	private:
		delegate<input::device_added_event> mDeviceAdded;
		delegate<input::device_removed_event> mDeviceRemoved;
		
		typedef std::pair<delegate<input::device_added_event>::return_t,
				delegate<input::device_removed_event>::return_t> pair_t;
		typedef std::unordered_map<handle<input_driver>, pair_t> device_map_t;
		device_map_t mDHandler;
		delegate<input::driver_added_event>::return_t mVAHandler;
		delegate<input::driver_removed_event>::return_t mVRHandler;
	};
}
