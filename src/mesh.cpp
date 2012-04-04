#include "prototype/mesh.h"
#include <netlib/internal.h>
#include <netlib/ref_counted.h>
#include <algorithm>
#include <stdexcept>
#include <vector>

using namespace netlib;

namespace prototype
{
	PROTOTYPE_API size_t opengl_type_size(GLenum _type)
	{
		switch(_type)
		{
		case GL_FLOAT:
			return sizeof(float);

		case GL_DOUBLE:
			return sizeof(double);

		case GL_UNSIGNED_INT:
			return sizeof(unsigned int);

		case GL_UNSIGNED_SHORT:
			return sizeof(unsigned short);

		case GL_UNSIGNED_BYTE:
			return sizeof(unsigned char);

		case GL_INT:
			return sizeof(int);

		case GL_SHORT:
			return sizeof(short);

		case GL_BYTE:
			return sizeof(char);
		}

		return 0;
	}

	PROTOTYPE_API size_t opengl_element_count(GLenum _type, size_t _num_indices)
	{
		switch(_type)
		{
		case GL_POLYGON:
			return 1;

		case GL_POINTS:
			return _num_indices;

		case GL_LINES:
			return _num_indices/2;

		case GL_TRIANGLES:
			return _num_indices/3;

		case GL_QUADS:
			return _num_indices/4;

		case GL_TRIANGLE_FAN:
			return _num_indices-2;

		case GL_QUAD_STRIP:
			return (_num_indices-2)/2;

		case GL_LINE_STRIP:
			return _num_indices-1;

		case GL_LINE_LOOP:
			return _num_indices;
		}

		return 0;
	}

	//
	// vertex_element
	//
	
	vertex_element::vertex_element(GLenum _type, GLsizei _stride, GLsizei _offset, GLsizei _count, bool _norm)
		: mType(_type), mStride(_stride), mOffset(_offset), mCount(_count), mNorm(_norm)
	{
	}
	
	vertex_element::vertex_element()
		: mType(0), mStride(0), mOffset(0), mCount(0), mNorm(false)
	{
	}

	void vertex_element::bind(GLuint _idx) const
	{
		glVertexAttribPointer(_idx, (GLint)mCount, mType,
			mNorm ? GL_TRUE : GL_FALSE, (GLsizei)mStride, (const GLvoid*)mOffset);
		glEnableVertexAttribArray(_idx);
	}

	void vertex_element::unbind(GLuint _idx) const
	{
		glDisableVertexAttribArray(_idx);
	}

	//
	// vertex_buffer_internal
	//

	struct vertex_buffer_internal: public ref_counted
	{
		GLuint id;

		vertex_buffer_internal(): id(0)
		{
			acquire();
		}

		~vertex_buffer_internal()
		{
			if(id && glewGetContext())
				glDeleteBuffers(1, &id);
		}

		static vertex_buffer_internal *get(void *_p)
		{
			return internal::get<vertex_buffer_internal>(_p);
		}
	};

	//
	// vertex_buffer
	//

	vertex_buffer::vertex_buffer()
	{
		internal::create<vertex_buffer_internal>(mInternal);
	}

	vertex_buffer::vertex_buffer(GLenum _usage, size_t _sz)
	{
		internal::create<vertex_buffer_internal>(mInternal);
		if(create())
			set_data(_usage, _sz);
	}
	
	vertex_buffer::vertex_buffer(GLenum _usage, size_t _sz, const void *_buffer)
	{
		internal::create<vertex_buffer_internal>(mInternal);
		if(create())
			set_data(_usage, _sz, _buffer);
	}

	vertex_buffer::vertex_buffer(vertex_buffer const& _b)
	{
		vertex_buffer_internal *bi = vertex_buffer_internal::get(_b.mInternal);
		bi->acquire();
		mInternal = bi;
	}

	vertex_buffer::~vertex_buffer()
	{
		vertex_buffer_internal *bi = vertex_buffer_internal::get(mInternal);
		bi->release();
	}

	GLuint vertex_buffer::id() const
	{
		vertex_buffer_internal *bi = vertex_buffer_internal::get(mInternal);
		return bi->id;
	}

	bool vertex_buffer::valid() const
	{
		vertex_buffer_internal *bi = vertex_buffer_internal::get(mInternal);
		return bi->id != 0;
	}

	bool vertex_buffer::create()
	{
		vertex_buffer_internal *bi = vertex_buffer_internal::get(mInternal);
		if(bi->id)
			return false;

		glGenBuffers(1, &bi->id);
		return bi->id != 0;
	}

	void vertex_buffer::bind(GLenum _as) const
	{
		vertex_buffer_internal *bi = vertex_buffer_internal::get(mInternal);
		glBindBuffer(_as, bi->id);
	}

	void vertex_buffer::unbind(GLenum _as) const
	{
		glBindBuffer(_as, 0);
	}
		
	void vertex_buffer::set_data(GLenum _usage, size_t _sz, const void *_data) const
	{
		bind(GL_ARRAY_BUFFER);
		glBufferData(GL_ARRAY_BUFFER, _sz, _data, _usage);
		unbind(GL_ARRAY_BUFFER);
	}

	void vertex_buffer::update_data(size_t _start, size_t _cnt, const void *_buffer) const
	{
		bind(GL_ARRAY_BUFFER);
		glBufferSubData(GL_ARRAY_BUFFER, _start, _cnt, _buffer);
		unbind(GL_ARRAY_BUFFER);
	}

	void vertex_buffer::draw(GLenum _type, GLint _start, GLsizei _count) const
	{
		bind(GL_ARRAY_BUFFER);
		glDrawArrays(_type, _start, _count);
		unbind(GL_ARRAY_BUFFER);
	}

	void vertex_buffer::draw(GLenum _type, vertex_buffer const& _idx, vertex_element const& _iel, GLsizei _count) const
	{
		bind(GL_ARRAY_BUFFER);
		_idx.bind(GL_ELEMENT_ARRAY_BUFFER);
		glDrawElements(_type, _count, _iel.type(), (const GLvoid*)_iel.offset());
		_idx.unbind(GL_ELEMENT_ARRAY_BUFFER);
		unbind(GL_ARRAY_BUFFER);
	}

	void vertex_buffer::draw(GLenum _type, vertex_buffer const& _idx, vertex_element const& _iel, GLint _start, GLsizei _cnt) const
	{
		bind(GL_ARRAY_BUFFER);
		_idx.bind(GL_ELEMENT_ARRAY_BUFFER);
		glDrawRangeElements(_type, _start, _start+_cnt, _cnt, _iel.type(), (const GLvoid*)_iel.offset());
		_idx.unbind(GL_ELEMENT_ARRAY_BUFFER);
		unbind(GL_ARRAY_BUFFER);
	}

	vertex_buffer &vertex_buffer::operator =(vertex_buffer const& _s)
	{
		vertex_buffer_internal *osi = vertex_buffer_internal::get(mInternal);
		vertex_buffer_internal *si = vertex_buffer_internal::get(_s.mInternal);

		si->acquire();
		osi->release();

		mInternal = si;
		return *this;
	}

	//
	// vertex_array_internal
	//

	struct vertex_array_internal: public ref_counted
	{
		GLuint id;

		vertex_array_internal(): id(0)
		{
			acquire();
		}

		~vertex_array_internal()
		{
			if(id && glewGetContext())
				glDeleteVertexArrays(1, &id);
		}

		static vertex_array_internal *get(void *_p)
		{
			return internal::get<vertex_array_internal>(_p);
		}
	};

	//
	// vertex_array
	//
	
	vertex_array::vertex_array()
	{
		internal::create<vertex_array_internal>(mInternal);
	}

	vertex_array::vertex_array(vertex_array const& _a)
	{
		vertex_array_internal *vi = vertex_array_internal::get(_a.mInternal);
		vi->acquire();
		mInternal = vi;
	}

	vertex_array::~vertex_array()
	{
		vertex_array_internal *vi = vertex_array_internal::get(mInternal);
		vi->release();
	}

	GLuint vertex_array::id() const
	{
		vertex_array_internal *vi = vertex_array_internal::get(mInternal);
		return vi->id;
	}

	bool vertex_array::valid() const
	{
		vertex_array_internal *vi = vertex_array_internal::get(mInternal);
		return vi->id != 0;
	}

	bool vertex_array::create()
	{
		vertex_array_internal *vi = vertex_array_internal::get(mInternal);
		if(vi->id)
			return false;

		glGenVertexArrays(1, &vi->id);
		return vi->id != 0;
	}

	void vertex_array::bind() const
	{
		vertex_array_internal *vi = vertex_array_internal::get(mInternal);
		glBindVertexArray(vi->id);
	}

	void vertex_array::unbind() const
	{
		glBindVertexArray(0);
	}

	void vertex_array::draw(GLenum _type, GLint _start, GLsizei _count) const
	{
		bind();
		glDrawArrays(_type, _start, _count);
		unbind();
	}

	void vertex_array::draw(GLenum _type, vertex_element const& _iel, GLsizei _count) const
	{
		bind();
		glDrawElements(_type, _count, _iel.type(), (const GLvoid*)_iel.offset());
		unbind();
	}

	void vertex_array::draw(GLenum _type, vertex_element const& _iel, GLint _start, GLsizei _cnt) const
	{
		bind();
		glDrawRangeElements(_type, _start, _start+_cnt, _cnt, _iel.type(), (const GLvoid*)_iel.offset());
		unbind();
	}
	
	vertex_array &vertex_array::operator =(vertex_array const& _s)
	{
		vertex_array_internal *osi = vertex_array_internal::get(mInternal);
		vertex_array_internal *si = vertex_array_internal::get(_s.mInternal);

		si->acquire();
		osi->release();

		mInternal = si;
		return *this;
	}

	//
	// mesh
	//

	mesh::mesh(): mVertexType(GL_TRIANGLES), mIndexCount(0)
	{
	}

	mesh::mesh(GLenum _type): mVertexType(_type)
	{
	}
	
	void mesh::set_type(GLenum _type)
	{
		mVertexType = _type;
	}

	bool mesh::valid() const
	{
		return mVertexArray.valid();
	}

	bool mesh::create()
	{
		return mVertexArray.create();
	}
	
	void mesh::set_indices(GLsizei _cnt)
	{
		mIndexCount = _cnt;
	}

	void mesh::set_indices(prototype::vertex_buffer const& _buff, vertex_element const& _el, GLsizei _cnt)
	{
		if(!mVertexArray.valid())
			create();

		mIndexBuffer = _buff;
		mIndexElement = _el;
		mIndexCount = _cnt;

		mVertexArray.bind();
		_buff.bind(GL_ELEMENT_ARRAY_BUFFER);
		mVertexArray.unbind();
	}

	bool mesh::add_element(GLuint _attr, vertex_buffer const& _buff, vertex_element const& _el)
	{
		if(!mVertexArray.valid())
			create();

		auto it = mVertexElements.find(_attr);
		if(it != mVertexElements.end())
			return false;

		element e;
		e.buffer = _buff;
		e.elem = _el;

		mVertexElements.insert(elem_map_t::value_type(_attr, e));

		mVertexArray.bind();
		_buff.bind(GL_ARRAY_BUFFER);
		_el.bind(_attr);
		mVertexArray.unbind();
		return true;
	}

	void mesh::remove_element(GLuint _attr)
	{
		auto it = mVertexElements.find(_attr);
		if(it == mVertexElements.end())
			return;

		mVertexArray.bind();
		it->second.buffer.bind(GL_ARRAY_BUFFER);
		it->second.elem.unbind(_attr);
		mVertexArray.unbind();
		mVertexElements.erase(it);
	}

	void mesh::clear_elements()
	{
		mVertexArray.bind();
		for(auto it = mVertexElements.begin();
			it != mVertexElements.end(); it++)
			it->second.elem.unbind(it->first);
		mVertexArray.unbind();
		mVertexElements.clear();
	}

	void mesh::draw() const
	{
		if(mIndexBuffer.valid())
			mVertexArray.draw(mVertexType, mIndexElement, mIndexCount);
		else
			mVertexArray.draw(mVertexType, mIndexCount);
	}

	void mesh::draw(GLint _off, GLsizei _cnt) const
	{
		if(!mIndexCount)
			return;

		if(mIndexBuffer.valid())
			mVertexArray.draw(mVertexType, _off, _cnt);
		else
			mVertexArray.draw(mVertexType, mIndexElement, _off, _cnt);
	}
}