#include "prototype/shader.h"
#include <algorithm>

namespace prototype
{
	shader::shader(GLenum _type)
	{
		mID = glCreateShader(_type);
	}

	shader::shader(GLenum _type, std::string const& _src)
	{
		mID = glCreateShader(_type);
		set_source(_src);
	}

	shader::~shader()
	{
		glDeleteShader(mID);
	}

	std::string shader::source() const
	{
		GLint len;
		glGetShaderiv(mID, GL_SHADER_SOURCE_LENGTH, &len);

		std::string ret;
		ret.resize(len);
		glGetShaderSource(mID, len, NULL, (GLchar*)ret.data());

		return ret;
	}

	void shader::set_source(std::string const& _str)
	{
		GLint len = _str.size();
		GLchar *str = (GLchar*)_str.data();
		glShaderSource(mID, 1, (const GLchar**)&str, &len);
	}

	bool shader::compile()
	{
		glCompileShader(mID);
		return compiled();
	}

	bool shader::compiled() const
	{
		GLint status;
		glGetShaderiv(mID, GL_COMPILE_STATUS, &status);

		return status == GL_TRUE;
	}
	
	std::string shader::error()
	{
		GLint len;
		glGetShaderiv(mID, GL_INFO_LOG_LENGTH, &len);

		std::string ret;
		ret.resize(len);
		glGetShaderInfoLog(mID, len, NULL, (GLchar*)ret.data());
		return ret;
	}

	//

	shader_program::shader_program() : mCurrentAttr(0)
	{
		mID = glCreateProgram();
	}

	shader_program::~shader_program()
	{
		glDeleteProgram(mID);
	}

	void shader_program::activate()
	{
		glUseProgram(mID);
	}

	void shader_program::deactivate()
	{
		glUseProgram(0);
	}

	bool shader_program::attach_shader(shader_t const& _h)
	{
		mShaders.push_back(_h);
		glAttachShader(mID, _h->id());
		return true;
	}

	void shader_program::detach_shader(shader_t const& _h)
	{
		std::remove_if(mShaders.begin(), mShaders.end(),
			[this, _h](shader_t const& _t) -> bool
		{
			if(_t == _h)
			{
				glDetachShader(mID, _h->id());
				return true;
			}

			return false;
		});
	}

	bool shader_program::link()
	{
		glLinkProgram(mID);
		return linked();
	}

	bool shader_program::linked() const
	{
		GLint status;
		glGetProgramiv(mID, GL_LINK_STATUS, &status);
		return status == GL_TRUE;
	}
	
	std::string shader_program::error()
	{
		GLint len;
		glGetProgramiv(mID, GL_INFO_LOG_LENGTH, &len);

		std::string ret;
		ret.resize(len);
		glGetProgramInfoLog(mID, len, NULL, (GLchar*)ret.data());
		return ret;
	}
	
	size_t shader_program::bind_attribute(std::string const& _nm)
	{
		glBindAttribLocation(mID, mCurrentAttr, _nm.c_str());
		return mCurrentAttr++;
	}
}