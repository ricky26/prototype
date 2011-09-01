#include "prototype/mesh.h"
#include <algorithm>
#include <stdexcept>

namespace prototype
{
	size_t mesh::add_element(element const& _el)
	{
		size_t ret = mList.size();
		mList.push_back(_el);
		return ret;
	}

	void mesh::remove_element(size_t _idx)
	{
		mList.erase(mList.begin() + _idx);
	}

	const mesh::element &mesh::element_at(size_t _idx) const
	{
		return mList.at(_idx);
	}

	size_t mesh::find(std::string const& _nm) const
	{
		size_t i = 0;
		for(auto it = mList.begin(); it != mList.end(); it++)
		{
			if(it->name == _nm)
				return i;

			i++;
		}
		
		return invalid_index;
	}

	namespace render
	{
		PROTOTYPE_API void render_mesh(mesh::handle_t const& _msh, GLenum _type)
		{
			glDrawElements(_type, _msh->indices()->size()/sizeof(mesh::index_t)/3, GL_UNSIGNED_INT, (GLvoid*)0);
		}

		PROTOTYPE_API void update_buffer(GLenum _target, data::handle_t const& _data)
		{
			update_buffer(_target, _data, 0, _data->size());
		}

		PROTOTYPE_API void update_buffer(GLenum _target, data::handle_t const& _data, size_t _start, size_t _end)
		{
			glBufferSubData(GL_ARRAY_BUFFER, _start, _end-_start, _data->ptr());
		}

		PROTOTYPE_API GLuint create_buffer(GLenum _target, data::handle_t const& _data, GLenum _usage)
		{
			GLuint buffer;
			glGenBuffers(1, &buffer);

			glBindBuffer(_target, buffer);
			glBufferData(_target, _data->size(), _data->ptr(), _usage);
			return buffer;
		}

		PROTOTYPE_API void create_mesh_buffers(mesh::handle_t const& _msh, GLenum _usage, GLuint _buffers[2])
		{
			_buffers[0] = create_buffer(GL_ARRAY_BUFFER, _msh->vertices(), _usage);
			_buffers[1] = create_buffer(GL_ELEMENT_ARRAY_BUFFER, _msh->indices(), _usage);
		}

		PROTOTYPE_API void bind_mesh_element(mesh::handle_t const& _msh, GLenum _type, GLboolean _norm, size_t _src, size_t _dest)
		{
			const mesh::element &e = _msh->element_at(_src); // TODO: remove the 4, it is the size of a float, should generalize.
			glVertexAttribPointer(_dest, e.count, _type, _norm, e.stride-(e.count*4), (GLvoid*)(NULL + e.offset));
		}

		PROTOTYPE_API void bind_mesh_element(mesh::handle_t const& _msh, GLenum _type, GLboolean _norm, std::string const& _nm, size_t _dest)
		{
			size_t idx = _msh->find(_nm);
			if(idx == mesh::invalid_index)
				throw std::runtime_error("no such element " + _nm);

			bind_mesh_element(_msh, _type, _norm, idx, _dest);
		}
	}
}