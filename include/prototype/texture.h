#include "gl.h"
#include <netlib/internal.h>

#pragma once

namespace prototype
{
	class PROTOTYPE_API texture: public netlib::internalized
	{
	public:
		texture();
		using netlib::internalized::internalized;

		bool valid() const;
		bool create();

		GLuint id() const;

		void activate(GLint _idx=0) const;
		void deactivate(GLint _idx=0) const;

		inline void setData(const void *_data, GLsizei _w, GLsizei _h, GLenum _type,
			GLenum _format, GLenum _iformat=GL_RGBA, GLint _level=0, GLint _border=0)
		{
			return setData(_data, _w, _h, _type, GL_TEXTURE_2D, GL_TEXTURE_2D,
				_format, _iformat, _level, _border);
		}

	protected:
		void setData(const void *_data, GLsizei _w, GLsizei _h, GLenum _type,
			GLenum _bind, GLenum _target,
			GLenum _format, GLenum _iformat=GL_RGBA, GLint _level=0, GLint _border=0);

	};

	class PROTOTYPE_API renderbuffer: public netlib::internalized
	{
	public:
		renderbuffer();
		using netlib::internalized::internalized;

		renderbuffer(GLenum _internalformat, GLsizei _width, GLsizei _height);
		renderbuffer(GLsizei _samples, GLenum _internalformat,
			GLsizei _width, GLsizei _height);

		bool valid() const;
		bool create(GLenum _internalformat, GLsizei _width, GLsizei _height);
		bool create(GLsizei _samples, GLenum _internalformat,
			GLsizei _width, GLsizei _height);

		void activate() const;
		void deactivate() const;
	};

	class PROTOTYPE_API framebuffer: public netlib::internalized
	{
	public:
		framebuffer();
		using netlib::internalized::internalized;

		bool valid() const;
		bool create();

		void activate() const;
		void deactivate() const;

		void texture(GLenum _target, texture const& _t);
		void renderbuffer(GLenum _target, renderbuffer const& _b);
	};

}