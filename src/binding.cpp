#include <prototype/binding.h>
#include <algorithm>

namespace prototype
{
	//
	// trigger
	//

	trigger::trigger()
		: mValue(false)
	{
	}

	void trigger::update(bool _v)
	{
		if(mValue == _v)
			return;

		mValue = _v;

		triggered_event evt(this, _v);
		mTriggered(&evt);
	}

	//
	// triggered_event
	//

	triggered_event::triggered_event(prototype::trigger *_trg, bool _val)
		: event(_trg), mValue(_val)
	{
	}

	//
	// boolean_binding
	//

	boolean_binding::boolean_binding(bool _inv)
		: mInvert(_inv)
	{
	}

	boolean_binding::~boolean_binding()
	{
		if(mControl)
			mControl->unbind(this);
	}

	bool boolean_binding::value() const
	{
		if(!mControl)
			return false;

		return mControl->value() ^ mInvert;
	}

	void boolean_binding::update()
	{
		bool val = value();
		
		for(auto it = mTriggers.begin();
			it != mTriggers.end(); it++)
			(*it)->update(val);
	}

	bool boolean_binding::bind(handle<ctl_t> const& _ctl)
	{
		return _ctl->bind(this);
	}

	void boolean_binding::unbind()
	{
		if(mControl)
			mControl->unbind(this);
	}

	bool boolean_binding::add_trigger(trig_t _trig)
	{
		if(std::find(mTriggers.begin(), mTriggers.end(), _trig)
			!= mTriggers.end())
			return false;

		mTriggers.push_back(_trig);
		_trig->update(value());
		return true;
	}

	void boolean_binding::remove_trigger(trig_t _trig)
	{
		auto it = std::find(mTriggers.begin(), mTriggers.end(), _trig);
		if(it != mTriggers.end())
			mTriggers.erase(it);
	}

	//
	// boolean_input_control
	//

	boolean_input_control::boolean_input_control(handle<input_device> const& _dev, std::string const& _nm)
		: input_control(_dev, _nm), mValue(false)
	{
	}

	bool boolean_input_control::bind(boolean_binding *_binding)
	{
		auto it = std::find(mBindings.begin(), mBindings.end(), _binding);
		if(it != mBindings.end())
			return false;

		mBindings.push_back(_binding);
		_binding->mControl = this;
		_binding->update();
		return true;
	}

	void boolean_input_control::unbind(boolean_binding *_binding)
	{
		auto it = std::find(mBindings.begin(), mBindings.end(), _binding);
		if(it == mBindings.end())
			return;

		_binding->mControl = nullptr;
		mBindings.erase(it);
	}

	void boolean_input_control::set_value(bool _v)
	{
		if(mValue == _v)
			return;

		mValue = _v;
		std::for_each(mBindings.begin(), mBindings.end(), [](boolean_binding *_b) { _b->update(); });
	}

	//
	// linear_trigger
	//

	linear_trigger::linear_trigger(float _thresh)
		: trigger(), mThresh(_thresh)
	{
	}

	void linear_trigger::set_threshold(float _thr)
	{
		mThresh = _thr;
	}

	void linear_trigger::update(float _f)
	{
		trigger::update(_f > mThresh);
	}

	//
	// linear_binding
	//
	
	linear_binding::linear_binding(float _db, float _dt)
		: mDeadBottom(_db), mDeadTop(_dt)
	{
	}

	linear_binding::~linear_binding()
	{
	}

	float linear_binding::value() const
	{
		if(!mControl)
			return 0;

		float v = mControl->value();

		if(v < mDeadBottom)
			v = 0;
		else
		{
			v -= mDeadBottom;
			v = std::min<float>(v / mDeadTop-mDeadBottom, 1);
		}

		return v;
	}

	void linear_binding::update()
	{
		float val = value();
		
		for(auto it = mTriggers.begin();
			it != mTriggers.end(); it++)
			(*it)->update(val);
	}
		
	bool linear_binding::bind(handle<ctl_t> const& _ctl)
	{
		return _ctl->bind(this);
	}

	void linear_binding::unbind()
	{
		if(mControl)
			mControl->unbind(this);
	}

	bool linear_binding::add_trigger(trig_t _trig)
	{
		if(std::find(mTriggers.begin(), mTriggers.end(), _trig)
			!= mTriggers.end())
			return false;

		mTriggers.push_back(_trig);
		_trig->update(value());
		return true;
	}

	void linear_binding::remove_trigger(trig_t _trig)
	{
		auto it = std::find(mTriggers.begin(), mTriggers.end(), _trig);
		if(it != mTriggers.end())
			mTriggers.erase(it);
	}

	//
	// linear_input_control
	//

	linear_input_control::linear_input_control(handle<input_device> const& _dev, std::string const& _nm)
		: input_control(_dev, _nm), mValue(0)
	{
	}

	bool linear_input_control::bind(linear_binding *_binding)
	{
		auto it = std::find(mBindings.begin(), mBindings.end(), _binding);
		if(it != mBindings.end())
			return false;

		mBindings.push_back(_binding);
		_binding->mControl = this;
		_binding->update();
		return true;
	}

	void linear_input_control::unbind(linear_binding *_binding)
	{
		auto it = std::find(mBindings.begin(), mBindings.end(), _binding);
		if(it == mBindings.end())
			return;

		_binding->mControl = nullptr;
		mBindings.erase(it);
	}

	void linear_input_control::set_value(float _v)
	{
		if(mValue == _v)
			return;

		mValue = _v;
		std::for_each(mBindings.begin(), mBindings.end(), [](linear_binding *_b) { _b->update(); });
	}

	//
	// relative_input_control
	//
	
	relative_input_control::relative_input_control(handle<input_device> const& _dev, std::string const& _nm)
		: linear_input_control(_dev, _nm)
	{
	}

	void relative_input_control::add_value(float _v)
	{
		set_value(value() + _v);
	}

	void relative_input_control::accept_value()
	{
		set_value(0);
	}

	//
	// hat_binding
	//

	hat_binding::hat_binding(int _rot)
		: mRotation(_rot)
	{
	}
	
	hat_binding::~hat_binding()
	{
	}

	hat_direction hat_binding::value() const
	{
		if(!mControl)
			return hat_middle;

		hat_direction dir = mControl->value();
		if(dir == hat_middle || mRotation == 0)
			return dir;

		dir = (hat_direction)((((dir-1) + mRotation) % (hat_direction_count-1)) + 1);
		return dir;
	}
		
	bool hat_binding::bind(handle<ctl_t> const& _ctl)
	{
		return _ctl->bind(this);
	}

	void hat_binding::unbind()
	{
		if(mControl)
			mControl->unbind(this);
	}

	//
	// hat_input_control
	// 

	hat_input_control::hat_input_control(handle<input_device> const& _dev, std::string const& _nm)
		: input_control(_dev, _nm), mValue(hat_middle)
	{
	}

	bool hat_input_control::bind(hat_binding *_binding)
	{
		auto it = std::find(mBindings.begin(), mBindings.end(), _binding);
		if(it != mBindings.end())
			return false;

		mBindings.push_back(_binding);
		_binding->mControl = this;
		return true;
	}

	void hat_input_control::unbind(hat_binding *_binding)
	{
		auto it = std::find(mBindings.begin(), mBindings.end(), _binding);
		if(it == mBindings.end())
			return;

		_binding->mControl = nullptr;
		mBindings.erase(it);
	}

	void hat_input_control::set_value(hat_direction _v)
	{
		if(mValue == _v)
			return;

		mValue = _v;
	}
}