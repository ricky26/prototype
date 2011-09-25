#include "prototype.h"
#include "mesh.h"
#include "shader.h"
#include <netlib/ref_counted.h>
#include <functional>
#include <vector>

#pragma once

namespace prototype
{
	class PROTOTYPE_API model: public netlib::ref_counted
	{
	public:
		typedef netlib::handle<model> handle_t;
		typedef std::vector<handle_t> list_t;
		typedef std::function<void(handle_t const& _ptr)> fn_t;

		model(fn_t const& _fn=&model::default_draw);

		PROTOTYPE_INLINE fn_t function() const { return mFn; }
		PROTOTYPE_INLINE list_t const& children() const { return mChildren; }
		PROTOTYPE_INLINE matrix4f const& transform() const { return mTransform; }
		PROTOTYPE_INLINE matrix4f const& model_transform() const { return mMTransform; }

		void set_transform(matrix4f const& _mat);

		PROTOTYPE_INLINE void draw() const { mFn(this); }

		void add_part(handle_t const& _part);
		void remove_part(handle_t const& _part);

		void draw_children();

	protected:
		void update_transform();

		static PROTOTYPE_INLINE void default_draw(handle_t const& _h) { _h->draw_children(); }

	private:
		handle_t mParent;
		fn_t mFn;
		list_t mChildren;
		matrix4f mMTransform; // node -> model
		matrix4f mTransform; // node -> parent node
	};
}