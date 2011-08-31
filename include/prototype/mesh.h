#include "prototype.h"
#include "gl.h"
#include <netlib/data.h>
#include <string>
#include <vector>

#pragma once

namespace prototype
{
	using netlib::data;

	/**
	 * A class for storing a mesh.
	 */
	class PROTOTYPE_API mesh
	{
	public:
		struct element
		{
			PROTOTYPE_INLINE element(std::string const& _nm, size_t _stride=sizeof(float), size_t _offset=0, size_t _count=1)
				: name(_nm), stride(_stride), offset(_offset), count(_count) {}

			std::string name;
			size_t stride;
			size_t offset;
			size_t count;
		};

		static const size_t invalid_index = -1;

		typedef std::vector<element> elem_list_t;

		NETLIB_INLINE mesh() {}
		NETLIB_INLINE mesh(void *_data, size_t _sz) : mData(_data, _sz) {}

		NETLIB_INLINE void *ptr() const { return mData.ptr(); }
		NETLIB_INLINE size_t size() const { return mData.size(); }

		size_t add_element(element const& _el);
		void remove_element(size_t _idx);

		const element &element_at(size_t _idx) const;
		size_t find(std::string const& _nm) const;

	private:
		data mData;
		elem_list_t mList;
	};

	namespace render
	{
		/**
		 * Uploads mesh data into a buffer object.
		 *
		 * @note Requires that the buffer be bound to GL_ARRAY_BUFFER.
		 */
		PROTOTYPE_API void update_mesh_buffer(mesh const& _msh);
		PROTOTYPE_API void update_mesh_buffer(mesh const& _msh, size_t _start, size_t _end);

		/**
		 * Does the initial glBufferData call.
		 */
		PROTOTYPE_API void create_mesh_buffer(mesh const& _msh, GLenum _usage);

		// Shortcut for glGenBuffers/create_mesh_buffer.
		PROTOTYPE_API GLuint bind_mesh_buffer(mesh const& _msh, GLenum _usage);

		// Binds a mesh element using glVertexAttribPointer
		PROTOTYPE_API void bind_mesh_element(mesh const& _msh, GLenum _type, GLboolean _norm, size_t _src, size_t _dest);
		PROTOTYPE_API void bind_mesh_element(mesh const& _msh, GLenum _type, GLboolean _norm, std::string const& _nm, size_t _dest);
	}
}