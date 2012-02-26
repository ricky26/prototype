#include "gl.h"
#include <netlib/internal.h>

#pragma once

namespace prototype
{
	class PROTOTYPE_API texture: public netlib::internalized
	{
	public:
		texture();

		bool valid() const;
		bool create();

		void activate(size_t _idx) const;

		inline void setData(const void *_data, size_t _w, size_t _h, GLenum _type,
			GLenum _format, GLenum _iformat=GL_RGBA, GLint _level=0, GLint _border=0)
		{
			return setData(_data, _w, _h, _type, GL_TEXTURE_2D, GL_TEXTURE_2D,
				_format, _iformat, _level, _border);
		}

	protected:
		void setData(const void *_data, size_t _w, size_t _h, GLenum _type,
			GLenum _bind, GLenum _target,
			GLenum _format, GLenum _iformat=GL_RGBA, GLint _level=0, GLint _border=0);

	};

}