#include <vorbis/vorbisfile.h>
#include <netlib/bitstream.h>
#include "prototype/audio.h"
#include <iostream>

using namespace netlib;

namespace prototype
{
	static size_t bitstream_read(void *_ptr, size_t _sz, size_t _nmemb, void *_src)
	{
		bitstream *str = reinterpret_cast<bitstream*>(_src);
		return str->read(_ptr, _sz*_nmemb);
	}

	static ov_callbacks bitstream_callbacks = {
		bitstream_read,
		nullptr,
		nullptr,
		nullptr
	};

	static int ov_open(bitstream *_str, OggVorbis_File *vf, const char *initial, long ibytes)
	{
		return ov_open_callbacks(_str, vf, initial, ibytes, bitstream_callbacks);
	}

	PROTOTYPE_API sound load_ogg(bitstream *_str)
	{
		OggVorbis_File vf;
		int ret = ov_open(_str, &vf, nullptr, 0);
		if(ret < 0)
			return sound();

		vorbis_info *info = ov_info(&vf, -1);
		audio_format fmt(info->channels, 16, info->rate);

		std::vector<char> data;
		int sec;
		do
		{
			char buff[4096];
			ret = ov_read(&vf, buff, sizeof(buff), 0, 2, 1, &sec);
			if(ret == 0)
			{
				break;
			}
			else if(ret < 0)
			{
				// ERR0R!
				std::cerr << "ogg error " << ret << std::endl;
			}
			else
				data.insert(data.end(), buff, buff + ret);
		}
		while(true);

		ov_clear(&vf);
		return sound(fmt, netlib::data::copy(data.data(), data.size()));
	}
}