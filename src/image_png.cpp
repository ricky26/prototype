#include "prototype/image.h"
#include <png.h>
#include <memory>
#include <iostream>

using namespace netlib;

namespace prototype
{

	struct png_state
	{
		png_structp ptr;
		png_infop info, end;

		png_state(std::function<void(png_structp, const char*)> const& _fn)
			: info(nullptr), end(nullptr) {
				ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, *_fn.target<png_error_ptr>(), nullptr);
		};

		~png_state() { png_destroy_read_struct(&ptr, info ? &info : nullptr, end ? &end: nullptr); }

		operator png_structp() const { return ptr; }
	};

	static void bitstream_read(png_structp _png, png_bytep _ptr, png_size_t _size)
	{
		bitstream *bs = reinterpret_cast<bitstream*>(png_get_io_ptr(_png));
		bs->read_block(_ptr, _size);
	}

	PROTOTYPE_API texture load_png(bitstream *_strm)
	{
		bool failed = false;
		std::function<void(png_structp, const char*)> err_fn = [&](png_structp _png, const char *_err) { 
			std::cerr << _err << std::endl;
			failed = true;
		};

		png_state state(err_fn);
		if(!state.ptr)
			return texture();

		state.info = png_create_info_struct(state);
		if(!state.info)
			return texture();

		png_set_read_fn(state, _strm, bitstream_read);
		png_read_info(state, state.info);

		uint32_t w, h;
		int d, c;
		png_get_IHDR(state, state.info, &w, &h, &d, &c, nullptr, nullptr, nullptr);

		// xforms

		png_read_update_info(state, state.info);

		if(failed)
			return texture();

		png_size_t bytes = png_get_rowbytes(state, state.info);
		std::unique_ptr<png_byte[]> data(new png_byte[bytes]);

		std::unique_ptr<png_bytep[]> rows(new png_bytep[h]);
		for(int i = 0; i < h; i++)
				rows[h-1-i] = data.get() + i*bytes;

		png_read_image(state, rows.get());

		texture ret;
		ret.setData(data.get(), w, h, GL_BYTE, GL_RGBA);
		return ret;
	}
}