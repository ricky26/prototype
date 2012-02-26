#include "prototype/texture.h"

namespace prototype
{
	struct texture_internal: public internal
	{
		GLuint id;

		texture_internal(): id(0)
		{
		}

		~texture_internal()
		{
			if(id && glewGetContext())
			{
				glDeleteTextures(1, &id);
			}
		}
	};

	texture::texture()
		: internalized(internalized::create<texture_internal>())
	{
	}
	
	bool texture::valid() const
	{
		texture_internal *i = get<texture_internal>();
		return i->id != 0;
	}

	bool texture::create()
	{
		texture_internal *i = get<texture_internal>();
		if(i->id)
			return false;

		glGenTextures(1, &i->id);
		return true;
	}

	void texture::activate(size_t _idx) const
	{
		texture_internal *i = get<texture_internal>();

		glActiveTexture(GL_TEXTURE0 + _idx);
		glBindTexture(GL_TEXTURE_2D, i->id);
	}

	void texture::setData(const void *_data, size_t _w, size_t _h, GLenum _type,
		GLenum _bind, GLenum _target,
		GLenum _format, GLenum _iformat, GLint _level, GLint _border)
	{
		texture_internal *i = get<texture_internal>();

		glBindTexture(_bind, i->id);
		glTexImage2D(_target, _level, _iformat, _w, _h, _border, _format, _type, _data);
		glBindTexture(_bind, 0);
	}
}