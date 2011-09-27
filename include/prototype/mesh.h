#include "prototype.h"
#include "gl.h"
#include <netlib/data.h>
#include <netlib/ref_counted.h>
#include <map>

#pragma once

namespace prototype
{
	using netlib::ref_counted;
	using netlib::handle;
	using netlib::data;
	
	PROTOTYPE_API size_t opengl_type_size(GLenum _type);
	PROTOTYPE_API size_t opengl_element_count(GLenum _type, size_t _num_indices);

	class PROTOTYPE_API vertex_element
	{
	public:
		vertex_element();
		vertex_element(GLenum _type, size_t _stride, size_t _offset, size_t _count=1, bool _norm=false);

		PROTOTYPE_INLINE GLenum type() const { return mType; }
		PROTOTYPE_INLINE size_t size() const { return opengl_type_size(mType); }
		PROTOTYPE_INLINE size_t stride() const { return mStride; }
		PROTOTYPE_INLINE size_t offset() const { return mOffset; }
		PROTOTYPE_INLINE size_t count() const { return mCount; }
		PROTOTYPE_INLINE bool normalize() const { return mNorm; }

		void bind(GLuint _idx) const;
		void unbind(GLuint _idx) const;

	private:
		GLenum mType;
		size_t mStride;
		size_t mOffset;
		size_t mCount;
		bool mNorm;
	};

	class PROTOTYPE_API vertex_buffer
	{
	public:
		vertex_buffer();
		vertex_buffer(GLenum _usage, size_t _sz);
		vertex_buffer(GLenum _usage, size_t _sz, const void *_buffer);
		vertex_buffer(vertex_buffer const& _b);
		~vertex_buffer();

		GLuint id() const;
		bool valid() const;

		bool create();

		void bind(GLenum _as) const;
		void unbind(GLenum _as) const;
		
		void set_data(GLenum _usage, size_t _sz, const void *_data) const;
		NETLIB_INLINE void set_data(GLenum _usage, size_t _sz) const { set_data(_usage, _sz, NULL); }

		void update_data(size_t _start, size_t _cnt, const void *_buffer) const;
		NETLIB_INLINE void update_data(size_t _sz, const void *_buffer) const { update_data(0, _sz, _buffer); }
		
		void draw(GLenum _type, size_t _start, size_t _count) const;
		PROTOTYPE_INLINE void draw(GLenum _type, size_t _count) const { draw(_type, 0, _count); }
		void draw(GLenum _type, vertex_buffer const& _idx, vertex_element const& _iel, size_t _start, size_t _cnt) const;
		PROTOTYPE_INLINE void draw(GLenum _type, vertex_element const& _iel, size_t _start, size_t _cnt) const { draw(_type, *this, _iel, _start, _cnt); }
		void draw(GLenum _type, vertex_buffer const& _idx, vertex_element const& _iel, size_t _count) const;
		PROTOTYPE_INLINE void draw(GLenum _type, vertex_element const& _iel, size_t _count) const { draw(_type, *this, _iel, _count); }
		
		vertex_buffer &operator =(vertex_buffer const& _b);
		PROTOTYPE_INLINE bool operator ==(vertex_buffer const& _b) const { return mInternal == _b.mInternal; }
		PROTOTYPE_INLINE bool operator !=(vertex_buffer const& _b) const { return mInternal != _b.mInternal; }

	private:
		void *mInternal;
	};

	class PROTOTYPE_API vertex_array
	{
	public:
		vertex_array();
		vertex_array(vertex_array const& _a);
		~vertex_array();

		GLuint id() const;
		bool valid() const;

		bool create();

		void bind() const;
		void unbind() const;
		
		void draw(GLenum _type, size_t _start, size_t _count) const;
		PROTOTYPE_INLINE void draw(GLenum _type, size_t _count) const { draw(_type, 0, _count); }
		void draw(GLenum _type, vertex_element const& _iel, size_t _start, size_t _cnt) const;
		void draw(GLenum _type, vertex_element const& _iel, size_t _count) const;
		
		vertex_array &operator =(vertex_array const& _b);
		PROTOTYPE_INLINE bool operator ==(vertex_array const& _b) const { return mInternal == _b.mInternal; }
		PROTOTYPE_INLINE bool operator !=(vertex_array const& _b) const { return mInternal != _b.mInternal; }

	private:
		void *mInternal;
	};

	class PROTOTYPE_API mesh
	{
	public:
		struct element
		{
			vertex_buffer buffer;
			vertex_element elem;
		};

		typedef std::map<GLuint, element> elem_map_t;
		
		mesh();
		mesh(GLenum _mode);

		bool valid() const;
		bool create();

		PROTOTYPE_INLINE GLenum type() const { return mVertexType; }
		void set_type(GLenum _type);

		PROTOTYPE_INLINE prototype::vertex_array const& vertex_array() const { return mVertexArray; }
		PROTOTYPE_INLINE prototype::vertex_buffer const& index_buffer() const { return mIndexBuffer; }
		PROTOTYPE_INLINE vertex_element const& index_element() const { return mIndexElement; }
		PROTOTYPE_INLINE size_t index_count() const { return mIndexCount; }
		void set_indices(size_t _cnt);
		void set_indices(prototype::vertex_buffer const& _buff, vertex_element const& _el, size_t _cnt);

		PROTOTYPE_INLINE elem_map_t const& elements() const { return mVertexElements; }
		bool add_element(GLuint _attr, vertex_buffer const& _buff, vertex_element const& _el);
		void remove_element(GLuint _attr);
		void clear_elements();

		void draw() const;
		void draw(size_t _off, size_t _cnt) const;

	private:
		GLenum mVertexType;
		prototype::vertex_buffer mIndexBuffer;
		vertex_element mIndexElement;
		size_t mIndexCount;
		elem_map_t mVertexElements;
		prototype::vertex_array mVertexArray;
	};
}