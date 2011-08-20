#include <prototype/window.h>

namespace prototype
{
	window_event::window_event(prototype::window *_win): event(_win)
	{
	}

	key_event::key_event(prototype::window *_sender, prototype::key _idx, bool _down)
		: window_event(_sender), mButton(_idx), mDown(_down)
	{
	}

	character_event::character_event(prototype::window *_sender, wchar_t _chr)
		: window_event(_sender), mChar(_chr)
	{
	}

	mouse_event::mouse_event(prototype::window *_sender, int _x, int _y)
		: window_event(_sender), mX(_x), mY(_y)
	{
	}
}