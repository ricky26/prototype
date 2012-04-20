#include "prototype/audio.h"
#include <algorithm>

using namespace netlib;

namespace prototype
{
	//
	// audio_driver
	//

	audio_driver::audio_driver(std::string const& _nm): driver(_nm)
	{
		driver_list().push_back(this);
	}

	audio_driver::list_t &audio_driver::driver_list()
	{
		static list_t list;
		return list;
	}

	//
	// channel
	//
	
	channel::channel()
	{
	}

	//
	// soundscape
	//
	
	soundscape::soundscape(handle<audio_driver> const& _drv)
		: mDriver(_drv)
	{
	}

	//
	// sound
	//

	sound::sound()
	{
	}

	sound::sound(audio_format const& _fmt, netlib::data &&_data)
		: mFormat(_fmt), mData(std::move(_data))
	{
	}

	sound::sound(audio_format const& _fmt, handle<netlib::data> const& _data)
		: mFormat(_fmt), mData(_data)
	{
	}

	sound::sound(audio_format const& _fmt)
		: mFormat(_fmt)
	{
	}
}
