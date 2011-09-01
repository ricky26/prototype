#include "prototype.h"
#include "gl.h"
#include <netlib/ref_counted.h>
#include <vector>
#include <string>

#pragma once

namespace prototype
{
	using netlib::ref_counted;
	using netlib::handle;

	class PROTOTYPE_API shader: public ref_counted
	{
	public:
		typedef handle<shader> handle_t;

		shader(GLenum _type);
		shader(GLenum _type, std::string const& _src);
		virtual ~shader();

		NETLIB_INLINE GLuint id() const { return mID; }

		std::string source() const;
		void set_source(std::string const& _str);

		bool compile();
		bool compiled() const;

		std::string error();

	private:
		GLuint mID;
	};

	class PROTOTYPE_API shader_program: public ref_counted
	{
	public:
		typedef handle<shader_program> handle_t;
		typedef shader::handle_t shader_t;
		typedef std::vector<shader_t> list_t;

		shader_program();
		virtual ~shader_program();

		NETLIB_INLINE GLuint id() const { return mID; }

		void activate();
		void deactivate();

		bool attach_shader(shader_t const& _h);
		void detach_shader(shader_t const& _h);
		list_t const& shaders() const { return mShaders; }

		bool link();
		bool linked() const;

		std::string error();

		size_t bind_attribute(std::string const& _nm);

	private:
		GLuint mID;
		list_t mShaders;
		size_t mCurrentAttr;
	};
}
