#include "driver.h"

#pragma once

namespace prototype
{
	struct static_driver_registration
	{
		inline static_driver_registration(handle<driver> const& _drv)
		{
			_drv->acquire();
			static_drivers().push_back(_drv);
		}

		static inline std::list<handle<driver>> &static_drivers() { static std::list<handle<driver>> drivers; return drivers; }
	};
}