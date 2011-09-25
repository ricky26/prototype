#include "prototype/model.h"
#include <algorithm>

namespace prototype
{
	model::model(fn_t const& _fn)
		: mTransform(matrix4f::identity()),
		mMTransform(matrix4f::identity()),
		mFn(_fn)
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

		_part->mParent = NULL;
		_part->update_transform();
	}

	void model::draw_children()
	{
		std::for_each(mChildren.begin(), mChildren.end(),
			[](handle_t const& _p) { _p->draw(); });
	}
}
