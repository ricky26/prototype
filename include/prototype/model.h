#include "prototype.h"
#include "mesh.h"
#include "shader.h"
#include <netlib/ref_counted.h>
#include <functional>
#include <vector>

#pragma once

namespace prototype
{
	namespace render_state
	{
		PROTOTYPE_API matrix4f const& world_matrix();
		PROTOTYPE_API void set_world_matrix(matrix4f const&);

		PROTOTYPE_API matrix4f const& view_matrix();
		PROTOTYPE_API void set_view_matrix(matrix4f const&);

		PROTOTYPE_API matrix4f const& projection_matrix();
		PROTOTYPE_API void set_projection_matrix(matrix4f const&);

		// Generated
		PROTOTYPE_API matrix4f const& view_projection_matrix();
		PROTOTYPE_API matrix4f const& world_projection_matrix();
	}

	template<typename T,
		T const&(*Y)(), void(*Z)(T const&)>
	class render_state_push
	{
	public:
		inline render_state_push(T const& _val)
		{
			save = Y();
			Z(_val);
		}

		inline ~render_state_push()
		{
			Z(save);
		}

	private:
		T save;
	};

	typedef render_state_push<matrix4f, render_state::world_matrix, render_state::set_world_matrix> world_matrix_push;

	class PROTOTYPE_API model: public netlib::ref_counted
	{
	public:
		typedef netlib::handle<model> handle_t;
		typedef std::vector<handle_t> list_t;

		model();
		virtual ~model();

		PROTOTYPE_INLINE list_t const& children() const { return mChildren; }
		PROTOTYPE_INLINE matrix4f const& transform() const { return mTransform; }
		PROTOTYPE_INLINE matrix4f const& model_transform() const { return mMTransform; }

		void set_transform(matrix4f const& _mat);

		virtual void draw() const;

		void add_part(handle_t const& _part);
		void remove_part(handle_t const& _part);

		void draw_children() const;

	protected:
		void update_transform();

	private:
		handle_t mParent;
		list_t mChildren;
		matrix4f mMTransform; // node -> model
		matrix4f mTransform; // node -> parent node
	};
}