#include "prototype.h"
#include "gl.h"
#include <netlib/ref_counted.h>
#include <netlib/internal.h>
#include <vector>
#include <string>
#include <functional>

#pragma once

namespace prototype
{
	class texture;
	class PROTOTYPE_API shader: public netlib::internalized
	{
	public:
		using internalized::internalized;

		shader();
		shader(GLenum _type);
		shader(GLenum _type, std::string const& _src);
		
		bool valid() const;
		GLuint id() const;
		
		bool create(GLenum _type);
		bool create(GLenum _type, std::string const& _src);

		std::string source() const;
		void set_source(std::string const& _str);

		bool compile();
		bool compiled() const;

		std::string error();
	};

	class PROTOTYPE_API shader_program: public netlib::internalized
	{
	public:
		using internalized::internalized;

		typedef std::vector<shader> list_t;

		shader_program();

		bool valid() const;
		GLuint id() const;

		bool create();

		void activate() const;
		void deactivate() const;

		bool attach_shader(shader const& _h);
		void detach_shader(shader const& _h);
		list_t const& shaders() const;

		bool link();
		bool linked() const;

		std::string error() const;

		bool bind_attribute(std::string const& _nm, GLuint _attr);
		GLuint find_attribute(std::string const& _nm) const;
		GLuint find_uniform(std::string const& _nm) const;
	};

	class PROTOTYPE_API shader_instance: public ref_counted
	{
	public:
		typedef handle<shader_instance> handle_t;
		typedef std::function<void(handle_t const&)> fn_t;

		PROTOTYPE_INLINE shader_instance() {}
		shader_instance(shader_program const& _p);
		shader_instance(shader_instance const& _sh);

		PROTOTYPE_INLINE bool valid() const { return mProgram.valid(); }
		PROTOTYPE_INLINE shader_program const& program() const { return mProgram; }

		inline virtual void activate() const;
		inline virtual void deactivate() const;

		static handle_t from_function(shader_program const& _p, fn_t const& _begin, fn_t const& _end=fn_t());

	private:
		shader_program mProgram;
	};
}
