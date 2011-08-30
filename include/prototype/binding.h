#include "input.h"
#include <netlib/event.h>
#include <list>

#pragma once

namespace prototype
{
	using netlib::delegate;
	using netlib::event;

	class PROTOTYPE_API trigger
	{
	public:
		trigger();

		void update(bool _v);

		PROTOTYPE_INLINE delegate<> &on_triggered() { return mTriggered; }

	private:
		bool mValue;
		delegate<> mTriggered;
	};

	class PROTOTYPE_API triggered_event: public event
	{
	public:
		triggered_event(trigger *_trg, bool _val);

		PROTOTYPE_INLINE prototype::trigger *trigger() const
		{
			return static_cast<prototype::trigger*>(sender());
		}

		PROTOTYPE_INLINE bool value() const { return mValue; }

	private:
		bool mValue;
	};

	//
	// boolean input
	//

	class boolean_input_control;
	class PROTOTYPE_API boolean_binding
	{
	public:
		friend class boolean_input_control;

		typedef trigger *trig_t;
		typedef std::list<trig_t> triggers_t;
		typedef boolean_input_control ctl_t;

		boolean_binding(bool _inv=false);
		virtual ~boolean_binding();

		PROTOTYPE_INLINE handle<ctl_t> const& control() const { return mControl; }

		bool value() const;
		void update();

		bool bind(handle<ctl_t> const& _ctl);
		void unbind();

		bool add_trigger(trig_t);
		void remove_trigger(trig_t);

	private:
		bool mInvert;
		handle<ctl_t> mControl;
		triggers_t mTriggers;
	};

	class PROTOTYPE_API boolean_input_control: public input_control
	{
	public:
		typedef std::list<boolean_binding*> bindings_t;

		boolean_input_control(handle<input_device> const& _dev, std::string const& _nm);

		PROTOTYPE_INLINE bool value() const { return mValue; }
		void set_value(bool _v);

		bool bind(boolean_binding *_binding);
		void unbind(boolean_binding *_binding);

	private:
		bool mValue;
		bindings_t mBindings;
	};

	//
	// linear input
	//

	class PROTOTYPE_API linear_trigger: public trigger
	{
	public:
		linear_trigger(float _thresh=.5f);

		PROTOTYPE_INLINE float threshold() const { return mThresh; }
		void set_threshold(float _thr);

		void update(float _f);

	private:
		float mThresh;
	};

	class linear_input_control;
	class PROTOTYPE_API linear_binding
	{
	public:
		friend class linear_input_control;

		typedef linear_trigger *trig_t;
		typedef std::list<trig_t> triggers_t;
		typedef linear_input_control ctl_t;

		linear_binding(float _db=0, float _dt=1);
		virtual ~linear_binding();

		PROTOTYPE_INLINE handle<ctl_t> const& control() const { return mControl; }

		float value() const;
		void update();
		
		bool bind(handle<ctl_t> const& _ctl);
		void unbind();

		bool add_trigger(trig_t);
		void remove_trigger(trig_t);

	private:
		float mDeadBottom;
		float mDeadTop;

		handle<ctl_t> mControl;
		triggers_t mTriggers;
	};

	class PROTOTYPE_API linear_input_control: public input_control
	{
	public:
		typedef std::list<linear_binding*> bindings_t;

		linear_input_control(handle<input_device> const& _dev, std::string const& _nm);

		PROTOTYPE_INLINE float value() const { return mValue; }
		void set_value(float _v);

		bool bind(linear_binding *_binding);
		void unbind(linear_binding *_binding);

	private:
		float mValue;
		bindings_t mBindings;
	};

	//
	// relative input
	//

	class PROTOTYPE_API relative_input_control: public linear_input_control
	{
	public:
		relative_input_control(handle<input_device> const& _dev, std::string const& _nm);

		void add_value(float _v);
		void accept_value();
	};

	//
	// hat input
	//

	enum hat_direction
	{
		hat_middle = 0,

		hat_nw,
		hat_n,
		hat_ne,
		hat_e,
		hat_se,
		hat_s,
		hat_sw,
		hat_w,

		hat_direction_count
	};

	class hat_input_control;
	class PROTOTYPE_API hat_binding
	{
	public:
		friend class hat_input_control;
		typedef hat_input_control ctl_t;

		hat_binding(int _rot=0);
		virtual ~hat_binding();

		PROTOTYPE_INLINE handle<ctl_t> const& control() const { return mControl; }

		hat_direction value() const;
		
		bool bind(handle<ctl_t> const& _ctl);
		void unbind();

	private:
		int mRotation;
		handle<ctl_t> mControl;
	};

	class PROTOTYPE_API hat_input_control: public input_control
	{
	public:
		typedef std::list<hat_binding*> bindings_t;

		hat_input_control(handle<input_device> const& _dev, std::string const& _str);

		PROTOTYPE_INLINE hat_direction value() const { return mValue; }
		void set_value(hat_direction _v);

		bool bind(hat_binding *_binding);
		void unbind(hat_binding *_binding);

	private:
		hat_direction mValue;
		bindings_t mBindings;
	};
}