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
		PROTOTYPE_API void update_mesh_buffer(mesh const& _msh)
		{
			update_mesh_buffer(_msh, 0, _msh.size());
		}

		PROTOTYPE_API void update_mesh_buffer(mesh const& _msh, size_t _start, size_t _end)
		{
			glBufferSubData(GL_ARRAY_BUFFER, _start, _end-_start, _msh.ptr());
		}

		PROTOTYPE_API void create_mesh_buffer(mesh const& _msh, GLenum _usage)
		{
			glBufferData(GL_ARRAY_BUFFER, _msh.size(), _msh.ptr(), _usage);
		}

		PROTOTYPE_API GLuint bind_mesh_buffer(mesh const& _msh, GLenum _usage)
		{
			GLuint buffer;
			glGenBuffers(1, &buffer);

			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			create_mesh_buffer(_msh, _usage);

			return buffer;
		}

		PROTOTYPE_API void bind_mesh_element(mesh const& _msh, GLenum _type, GLboolean _norm, size_t _src, size_t _dest)
		{
			const mesh::element &e = _msh.element_at(_src);
			glVertexAttribPointer(_dest, e.count, _type, _norm, e.stride, (GLvoid*)(NULL + e.offset));
		}

		PROTOTYPE_API void bind_mesh_element(mesh const& _msh, GLenum _type, GLboolean _norm, std::string const& _nm, size_t _dest)
		{
			size_t idx = _msh.find(_nm);
			if(idx = mesh::invalid_index)
				throw std::runtime_error("no such element " + _nm);

			bind_mesh_element(_msh, _type, _norm, idx, _dest);
		}
	}
}