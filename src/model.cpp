#include "prototype/model.h"
#include <algorithm>

namespace prototype
{
	model::model()
		: mTransform(matrix4f::identity()),
		mMTransform(matrix4f::identity())
	{
	}

	model::~model()
	{
	}

	void model::set_transform(matrix4f const& _mat)
	{
		mTransform = _mat;
		update_transform();
	}

	void model::update_transform()
	{
		if(mParent)
			mMTransform = mParent->transform()*mTransform;
		else
			mMTransform = mTransform;

		std::for_each(mChildren.begin(), mChildren.end(),
			[](handle_t const& _p) { _p->update_transform(); });
	}

	void model::add_part(handle_t const& _part)
	{
		if(_part->mParent)
			return;

		mChildren.push_back(_part);
		_part->mParent = this;
		_part->update_transform();
	}

	void model::remove_part(handle_t const& _part)
	{
		if(_part->mParent != this)
			return;

		_part->mParent = nullptr;
		_part->update_transform();
	}

	void model::draw() const
	{
		draw_children();
	}

	void model::draw_children() const
	{
		std::for_each(mChildren.begin(), mChildren.end(),
			[](handle_t const& _p) { _p->draw(); });
	}

	namespace render_state
	{
		struct render_state_t
		{
			matrix4f world_mat;
			matrix4f view_mat;
			matrix4f proj_mat;

			matrix4f view_proj_mat;
			matrix4f world_proj_mat;
		};

		PROTOTYPE_THREAD render_state_t *render_state = nullptr;

		static inline render_state_t *get_state()
		{
			if(!render_state)
			{
				render_state = new render_state_t;
				render_state->world_mat = matrix4f::identity();
				render_state->view_mat = matrix4f::identity();
				render_state->proj_mat = matrix4f::identity();
			}

			return render_state;
		}

		PROTOTYPE_API matrix4f const& world_matrix()
		{
			return get_state()->world_mat;
		}

		PROTOTYPE_API void set_world_matrix(matrix4f const& _m)
		{
			get_state()->world_mat = _m;
			get_state()->world_proj_mat = get_state()->view_proj_mat * _m;
		}

		PROTOTYPE_API matrix4f const& view_matrix()
		{
			return get_state()->view_mat;
		}

		PROTOTYPE_API void set_view_matrix(matrix4f const& _m)
		{
			get_state()->view_mat = _m;
			get_state()->view_proj_mat = get_state()->proj_mat * _m;
			get_state()->world_proj_mat =
				get_state()->view_proj_mat * get_state()->world_mat;
		}

		PROTOTYPE_API matrix4f const& projection_matrix()
		{
			return get_state()->proj_mat;
		}

		PROTOTYPE_API void set_projection_matrix(matrix4f const& _m)
		{
			get_state()->proj_mat = _m;
			get_state()->view_proj_mat = _m * get_state()->view_mat;
			get_state()->world_proj_mat =
				get_state()->view_proj_mat * get_state()->world_mat;
		}

		PROTOTYPE_API matrix4f const& view_projection_matrix()
		{
			return get_state()->view_proj_mat;
		}

		PROTOTYPE_API matrix4f const& world_projection_matrix()
		{
			return get_state()->world_proj_mat;
		}
	}
}
