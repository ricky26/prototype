#include "prototype/texture.h"

namespace prototype
{
	//
	// texture
	//

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

	void texture::activate(GLint _idx) const
	{
		texture_internal *i = get<texture_internal>();

		glActiveTexture(GL_TEXTURE0 + _idx);
		glBindTexture(GL_TEXTURE_2D, i->id);
	}

	void texture::deactivate(GLint _idx) const
	{
		glActiveTexture(GL_TEXTURE0 + _idx);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void texture::setData(const void *_data, GLsizei _w, GLsizei _h, GLenum _type,
		GLenum _bind, GLenum _target,
		GLenum _format, GLenum _iformat, GLint _level, GLint _border)
	{
		texture_internal *i = get<texture_internal>();

		glBindTexture(_bind, i->id);
		glTexImage2D(_target, _level, _iformat, _w, _h, _border, _format, _type, _data);
		glBindTexture(_bind, 0);
	}

	//
	// renderbuffer
	//

	struct renderbuffer_internal: public internal
	{
		GLuint id;

		renderbuffer_internal(): id(0)
		{
		}

		~renderbuffer_internal()
		{
			if(id && glewGetContext())
			{
				glDeleteRenderbuffers(1, &id);
			}
		}
	};

	renderbuffer::renderbuffer()
		: internalized(internalized::create<renderbuffer_internal>())
	{
	}

	renderbuffer::renderbuffer(GLenum _internalformat, GLsizei _width, GLsizei _height)
		: internalized(internalized::create<renderbuffer_internal>())
	{
		create(_internalformat, _width, _height);
	}

	renderbuffer::renderbuffer(GLsizei _samples, GLenum _internalformat,
		GLsizei _width, GLsizei _height)
		: internalized(internalized::create<renderbuffer_internal>())
	{
		create(_samples, _internalformat, _width, _height);
	}

	bool renderbuffer::valid() const
	{
		renderbuffer_internal *i = get<renderbuffer_internal>();
		return i && i->id;
	}

	bool renderbuffer::create(GLenum _internalformat, GLsizei _width, GLsizei _height)
	{
		if(valid())
			return true;
		
		renderbuffer_internal *i = get<renderbuffer_internal>();
		glGenRenderbuffers(1, &i->id);
		if(!i->id)
			return false;

		activate();
		glRenderbufferStorage(GL_RENDERBUFFER, _internalformat, _width, _height);
		return true;
	}

	bool renderbuffer::create(GLsizei _samples, GLenum _internalformat,
		GLsizei _width, GLsizei _height)
	{
		if(valid())
			return true;
		
		renderbuffer_internal *i = get<renderbuffer_internal>();
		glGenRenderbuffers(1, &i->id);
		if(!i->id)
			return false;

		activate();
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, _samples, _internalformat, _width, _height);
		return true;
	}

	void renderbuffer::activate() const
	{
		renderbuffer_internal *i = get<renderbuffer_internal>();
		glBindRenderbuffer(GL_RENDERBUFFER, i->id);
	}

	void renderbuffer::deactivate() const
	{
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
}