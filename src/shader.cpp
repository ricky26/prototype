#include "prototype/shader.h"
#include <netlib/ref_counted.h>
#include <netlib/internal.h>
#include <algorithm>

using namespace netlib;

namespace prototype
{
	//
	// shader_internal
	//

	struct shader_internal: public ref_counted
	{
		GLuint id;

		shader_internal(): id(0)
		{
			acquire();
		}

		~shader_internal()
		{
			if(id && glewGetContext())
				glDeleteShader(id);
		}

		static shader_internal *get(void *_p)
		{
			return internal::get<shader_internal>(_p);
		}
	};

	shader::shader()
		: internalized(internalized::create<shader_internal>())
	{
	}

	shader::shader(GLenum _type)
		: internalized(internalized::create<shader_internal>())
	{
		create(_type);
	}

	shader::shader(GLenum _type, std::string const& _src)
		: internalized(internalized::create<shader_internal>())
	{
		create(_type);
		set_source(_src);
	}
	
	bool shader::valid() const
	{
		shader_internal *si = get<shader_internal>();
		return si->id != 0;
	}

	GLuint shader::id() const
	{
		shader_internal *si = get<shader_internal>();
		return si->id;
	}

	bool shader::create(GLenum _type)
	{
		shader_internal *si = get<shader_internal>();
		if(si->id)
			return false;

		si->id = glCreateShader(_type);
		return si->id != 0;
	}

	std::string shader::source() const
	{
		shader_internal *si = get<shader_internal>();

		GLint len;
		glGetShaderiv(si->id, GL_SHADER_SOURCE_LENGTH, &len);

		std::string ret;
		ret.resize(len);
		glGetShaderSource(si->id, len, NULL, (GLchar*)ret.data());

		return ret;
	}

	void shader::set_source(std::string const& _str)
	{
		shader_internal *si = get<shader_internal>();

		GLint len = _str.size();
		GLchar *str = (GLchar*)_str.data();
		glShaderSource(si->id, 1, (const GLchar**)&str, &len);
	}

	bool shader::compile()
	{
		shader_internal *si = get<shader_internal>();
		glCompileShader(si->id);
		return compiled();
	}

	bool shader::compiled() const
	{
		shader_internal *si = get<shader_internal>();

		GLint status;
		glGetShaderiv(si->id, GL_COMPILE_STATUS, &status);

		return status == GL_TRUE;
	}
	
	std::string shader::error()
	{
		shader_internal *si = get<shader_internal>();

		GLint len;
		glGetShaderiv(si->id, GL_INFO_LOG_LENGTH, &len);

		std::string ret;
		ret.resize(len);
		glGetShaderInfoLog(si->id, len, NULL, (GLchar*)ret.data());
		return ret;
	}

	//
	// shader_program_internal
	//

	struct shader_program_internal: public ref_counted
	{
		GLuint id;
		shader_program::list_t shaders;
		size_t current_attr;

		shader_program_internal(): id(0), current_attr(0)
		{
			acquire();
		}

		~shader_program_internal()
		{
			if(id && glewGetContext())
				glDeleteProgram(id);
		}

		static shader_program_internal *get(void *_p)
		{
			return internal::get<shader_program_internal>(_p);
		}
	};

	//

	shader_program::shader_program()
		: internalized(internalized::create<shader_program>())
	{
	}

	bool shader_program::valid() const
	{
		shader_program_internal *si = get<shader_program_internal>();
		return si->id != 0;
	}

	GLuint shader_program::id() const
	{
		shader_program_internal *si = get<shader_program_internal>();
		return si->id;
	}

	bool shader_program::create()
	{
		shader_program_internal *si = get<shader_program_internal>();
		if(si->id)
			return false;

		si->id = glCreateProgram();
		return si->id != 0;
	}
	
	shader_program::list_t const& shader_program::shaders() const
	{
		shader_program_internal *si = get<shader_program_internal>();
		return si->shaders;
	}

	void shader_program::activate() const
	{
		shader_program_internal *si = get<shader_program_internal>();
		glUseProgram(si->id);
	}

	void shader_program::deactivate() const
	{
		glUseProgram(0);
	}

	bool shader_program::attach_shader(shader const& _h)
	{
		shader_program_internal *si = get<shader_program_internal>();
		if(!si->id && !create())
			return false;

		si->shaders.push_back(_h);
		glAttachShader(si->id, _h.id());
		return true;
	}

	void shader_program::detach_shader(shader const& _h)
	{
		shader_program_internal *si = get<shader_program_internal>();

		std::remove_if(si->shaders.begin(), si->shaders.end(),
			[this, si, _h](shader const& _t) -> bool
		{
			if(_t == _h)
			{
				glDetachShader(si->id, _h.id());
				return true;
			}

			return false;
		});
	}

	bool shader_program::link()
	{
		shader_program_internal *si = get<shader_program_internal>();
		glLinkProgram(si->id);
		return linked();
	}

	bool shader_program::linked() const
	{
		shader_program_internal *si = get<shader_program_internal>();

		GLint status;
		glGetProgramiv(si->id, GL_LINK_STATUS, &status);
		return status == GL_TRUE;
	}
	
	std::string shader_program::error() const
	{
		shader_program_internal *si = get<shader_program_internal>();

		GLint len;
		glGetProgramiv(si->id, GL_INFO_LOG_LENGTH, &len);

		std::string ret;
		ret.resize(len);
		glGetProgramInfoLog(si->id, len, NULL, (GLchar*)ret.data());
		return ret;
	}
	
	bool shader_program::bind_attribute(std::string const& _nm, GLuint _attr)
	{
		shader_program_internal *si = get<shader_program_internal>();

		glBindAttribLocation(si->id, _attr, _nm.c_str());
		return glGetError() != GL_NO_ERROR;
	}
	
	GLuint shader_program::find_attribute(std::string const& _nm) const
	{
		shader_program_internal *si = get<shader_program_internal>();
		return glGetAttribLocation(si->id, _nm.c_str());
	}
	
	GLuint shader_program::find_uniform(std::string const& _nm) const
	{
		shader_program_internal *si = get<shader_program_internal>();
		return glGetUniformLocation(si->id, _nm.c_str());
	}

	//
	// shader_instance
	//

	shader_instance::shader_instance(shader_instance const& _sh)
	{
		mProgram = _sh.mProgram;
	}

	shader_instance::shader_instance(shader_program const& _p)
		: mProgram(_p)
	{
	}

	void shader_instance::activate() const
	{
		mProgram.activate();
	}

	void shader_instance::deactivate() const
	{
		mProgram.deactivate();
	}

	class functional_shader_instance: public shader_instance
	{
	public:
		functional_shader_instance(shader_program const& _p, fn_t const& _a, fn_t const& _b)
			: shader_instance(_p), mBegin(_a), mEnd(_b)
		{}

		void activate() const override
		{
			shader_instance::activate();
			mBegin(this);
		}

		void deactivate() const override
		{
			mEnd(this);
			shader_instance::deactivate();
		}

	private:
		fn_t mBegin, mEnd;
	};

	handle<shader_instance> shader_instance::from_function(shader_program const& _p, fn_t const& _b, fn_t const& _e)
	{
		return new functional_shader_instance(_p, _b, _e);
	}
}