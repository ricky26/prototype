#include "driver.h"
#include "netlib/data.h"
#include "netlib/bitstream.h"
#pragma once

namespace prototype
{
	class audio_driver;

	class PROTOTYPE_API audio_format
	{
	public:
		inline audio_format(): mChannels(-1), mBits(-1), mRate(-1) {}
		inline audio_format(int _c, int _b, int _r)
			: mChannels(_c), mBits(_b), mRate(_r) {}

		inline bool valid() const { return mChannels > 0; }

		inline int channels() const { return mChannels; }
		inline int bits() const { return mBits; }
		inline int rate() const { return mRate; }

	private:
		int mChannels;
		int mBits;
		int mRate;
	};

	class PROTOTYPE_API sound
	{
	public:
		sound();
		sound(audio_format const& _fmt, handle<netlib::data> const& _data);
		sound(audio_format const& _fmt);
		sound(sound&&);

		inline bool valid() const { return format().valid() && mData && data()->valid(); }

		inline audio_format const& format() const { return mFormat; }
		inline netlib::handle<netlib::data> const& data() const { return mData; }

		void set_data(handle<netlib::data> const& _data) { mData = _data; }

	private:
		sound(sound const&);
		audio_format mFormat;
		netlib::handle<netlib::data> mData;
	};

	class PROTOTYPE_API audio_channel: public ref_counted
	{
	public:
		virtual void start() = 0;
		virtual void stop() = 0;

		virtual void flush() = 0;

		virtual bool upload(void *_buffer, size_t _sz, bool _loop=false) = 0;
		virtual inline bool upload(sound const& _sound, bool _loop=false) { return upload(_sound.data()->ptr(), _sound.data()->size(), _loop); }

	protected:
		audio_channel();
	};

	/** Used in a similar way to window for the gl subsystem. */
	class PROTOTYPE_API soundscape: public ref_counted
	{
	public:
		virtual handle<audio_channel> create_channel(audio_format const& _f) = 0;

	protected:
		soundscape(handle<audio_driver> const& _drv);

	private:
		handle<audio_driver> mDriver;
	};

	class PROTOTYPE_API audio_driver: public driver
	{
	public:
		typedef handle<audio_driver> handle_t;
		typedef std::list<handle_t> list_t;

		virtual handle<soundscape> create_soundscape() = 0;
		
		static inline list_t const& drivers() { return driver_list(); };

	protected:
		audio_driver(std::string const& _nm);

	private:
		static list_t &driver_list();
	};

	// Loose functions

	PROTOTYPE_API sound load_ogg(netlib::bitstream *_str);
}
