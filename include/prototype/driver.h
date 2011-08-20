#include "prototype.h"
#include <netlib/ref_counted.h>
#include <string>
#include <unordered_map>

#pragma once

namespace prototype
{
	using netlib::ref_counted;
	using netlib::handle;

	class PROTOTYPE_API driver: public ref_counted
	{
	public:
		typedef std::unordered_map<std::string, handle<driver> > drivers_t;

		driver(std::string const& _nm);

		PROTOTYPE_INLINE std::string const& name() const { return mName; }

		bool register_driver();
		void unregister_driver();

		static bool register_static_drivers();

	protected:
		void destroy() override;

		virtual bool init();
		virtual void shutdown();

	private:
		std::string mName;
		bool mInitDone;
		bool mRegistered;

		static drivers_t gDrivers;
	};

}
