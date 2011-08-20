#include "driver.h"
#pragma once

namespace prototype
{
	class PROTOTYPE_API audio_driver: public driver
	{
	};

	/** Used in a similar way to window for the gl subsystem. */
	class PROTOTYPE_API soundscape
	{
	public:
		soundscape();
		soundscape(handle<audio_driver> const& _drv);
		~soundscape();

		bool valid() const;
		bool open(handle<audio_driver> const& _drv);
		void close();

	private:
		handle<audio_driver> mDriver;
	};
}
