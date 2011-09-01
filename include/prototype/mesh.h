#include "prototype.h"
#include "gl.h"
#include <netlib/data.h>
#include <netlib/ref_counted.h>
#include <string>
#include <vector>

#pragma once

namespace prototype
{
	using netlib::ref_counted;
	using netlib::handle;
	using netlib::data;

	/**
	 * A class for storing a mesh.
	 */
	class PROTOTYPE_API mesh: public ref_counted
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
		
		typedef handle<mesh> handle_t;
		typedef uint32_t index_t;
		typedef data::handle_t data_t;

		typedef std::vector<element> elem_list_t;

		static const size_t invalid_index = -1;

		NETLIB_INLINE mesh() {}
		NETLIB_INLINE mesh(data_t const& _vertices, data_t const& _indices): mData(_vertices), mIndices(_indices) {}

		NETLIB_INLINE data_t const& vertices() const { return mData; }
		NETLIB_INLINE void set_vertices(data_t const& _d) { mData = _d; }
		NETLIB_INLINE data_t const& indices() const { return mIndices; }
		NETLIB_INLINE void set_indices(data_t const& _d) { mIndices = _d; }

		size_t add_element(element const& _el);
		void remove_element(size_t _idx);

		const element &element_at(size_t _idx) const;
		size_t find(std::string const& _nm) const;

	private:
		data_t mData;
		data_t mIndices;
		elem_list_t mList;
	};

	namespace render
	{
		PROTOTYPE_API void render_mesh(mesh::handle_t const& _msh, GLenum _type);

		/**
		 * Uploads mesh data into a buffer object.
		 */
		PROTOTYPE_API void update_buffer(GLenum _target, data::handle_t const& _data);
		PROTOTYPE_API void update_buffer(GLenum _target, data::handle_t const& _data, size_t _start, size_t _end);

		/**
		 * Does the initial glBufferData call.
		 */
		PROTOTYPE_API GLuint create_buffer(GLenum _target, data::handle_t const& _msh, GLenum _usage);

		// Shortcut for glGenBuffers/create_mesh_buffer.
		PROTOTYPE_API void create_mesh_buffers(mesh::handle_t const& _msh, GLenum _usage, GLuint _buffers[2]);

		// Binds a mesh element using glVertexAttribPointer
		PROTOTYPE_API void bind_mesh_element(mesh::handle_t const& _msh, GLenum _type, GLboolean _norm, size_t _src, size_t _dest);
		PROTOTYPE_API void bind_mesh_element(mesh::handle_t const& _msh, GLenum _type, GLboolean _norm, std::string const& _nm, size_t _dest);
	}
}