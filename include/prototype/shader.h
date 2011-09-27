#include "prototype.h"
#include "gl.h"
#include <vector>
#include <string>

#pragma once

namespace prototype
{
	class PROTOTYPE_API shader
	{
	public:
		shader();
		shader(GLenum _type);
		shader(GLenum _type, std::string const& _src);
		shader(shader const& _b);
		~shader();
		
		bool valid() const;
		GLuint id() const;

		bool create(GLenum _type);

		std::string source() const;
		void set_source(std::string const& _str);

		bool compile();
		bool compiled() const;

		std::string error();
		
		shader &operator =(shader const& _b);
		PROTOTYPE_INLINE bool operator ==(shader const& _b) const { return mInternal == _b.mInternal; }
		PROTOTYPE_INLINE bool operator !=(shader const& _b) const { return mInternal != _b.mInternal; }

	private:
		void *mInternal;
	};

	class PROTOTYPE_API shader_program
	{
	public:
		typedef std::vector<shader> list_t;

		shader_program();
		shader_program(shader_program const& _p);
		~shader_program();

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
		
		shader_program &operator =(shader_program const& _b);
		PROTOTYPE_INLINE bool operator ==(shader_program const& _b) const { return mInternal == _b.mInternal; }
		PROTOTYPE_INLINE bool operator !=(shader_program const& _b) const { return mInternal != _b.mInternal; }

	private:
		void *mInternal;
	};
}
