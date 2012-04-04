#include "prototype.h"
#include <netlib/event.h>
#include <string>

#pragma once

namespace prototype
{
	using netlib::event;
	using netlib::delegate;

	enum key;
	class window;

	enum modifier
	{
		modifier_shift		= (1 << 0),
		modifier_alt		= (1 << 1),
		modifier_control	= (1 << 2),
	};

	//
	// events
	//

	class PROTOTYPE_API window_event: public event
	{
	public:
		window_event(window *_sender);

		PROTOTYPE_INLINE prototype::window *window() const
		{
			return static_cast<prototype::window*>(sender());
		}
	};

	class PROTOTYPE_API key_event: public window_event
	{
	public:
		key_event(prototype::window *_sender, key _idx, bool _down);

		PROTOTYPE_INLINE prototype::key key() const { return mButton; }
		PROTOTYPE_INLINE bool down() const { return mDown; }

	private:
		prototype::key mButton;
		bool mDown;
	};

	class PROTOTYPE_API character_event: public window_event
	{
	public:
		character_event(prototype::window *_sender, wchar_t _chr);

		PROTOTYPE_INLINE wchar_t character() const { return mChar; }

	private:
		wchar_t mChar;
	};

	class PROTOTYPE_API mouse_event: public window_event
	{
	public:
		mouse_event(prototype::window *_sender, int _x, int _y);
		
		PROTOTYPE_INLINE int x() const { return mX; }
		PROTOTYPE_INLINE int y() const { return mY; }

	private:
		int mX, mY;
	};

	//
	// window
	//

	class PROTOTYPE_API window
	{
	public:
		friend struct window_internal;

		window();
		window(window const&);
		window(window&&);
		window(std::string const& _title);
		virtual ~window();

		bool valid() const;
		bool create(std::string const& _title);

		std::string title() const;
		void set_title(std::string const& _title);

		int width() const;
		int height() const;
		void set_size(int _sx, int _sy);

		bool active() const;
		void activate();

		bool visible() const;
		void set_visible(bool _v);

		PROTOTYPE_INLINE void show() { set_visible(true); }
		PROTOTYPE_INLINE void hide() { set_visible(false); }
		
		bool cursor_visible() const;
		void set_cursor_visible(bool _v);
		
		PROTOTYPE_INLINE void show_cursor() { set_cursor_visible(true); }
		PROTOTYPE_INLINE void hide_cursor() { set_cursor_visible(false); }

		bool fullscreen(int _w, int _h, int _r);
		void minimise();
		void maximise();
		void restore();
		void close();

		bool begin_frame();
		void end_frame();

		bool grab_mouse();
		void release_mouse();

		void repaint();

		int modifiers() const;

		PROTOTYPE_INLINE netlib::delegate<window_event> &on_resized() { return mResized; }
		PROTOTYPE_INLINE netlib::delegate<window_event> &on_painted() { return mPainted; }
		PROTOTYPE_INLINE netlib::delegate<window_event> &on_closed() { return mClosed; }
		
		PROTOTYPE_INLINE netlib::delegate<key_event> &on_key_down() { return mKeyDown; }
		PROTOTYPE_INLINE netlib::delegate<key_event> &on_key_pressed() { return mKeyPressed; }
		PROTOTYPE_INLINE netlib::delegate<character_event> &on_key_typed() { return mKeyTyped; }
		PROTOTYPE_INLINE netlib::delegate<key_event> &on_key_up() { return mKeyUp; }
		
		PROTOTYPE_INLINE netlib::delegate<mouse_event> &on_mouse_move() { return mMouseMove; }
		PROTOTYPE_INLINE netlib::delegate<key_event> &on_mouse_down() { return mMouseDown; }
		PROTOTYPE_INLINE netlib::delegate<key_event> &on_mouse_double_click() { return mMouseDblClick; }
		PROTOTYPE_INLINE netlib::delegate<key_event> &on_mouse_up() { return mMouseUp; }

		static bool init();
		static void think();
		static void shutdown();

	protected:
		virtual void paint();
		virtual void closed();

		virtual void key_down(key _key);
		virtual void key_pressed(key _key);
		virtual void key_typed(wchar_t _chr);
		virtual void key_up(key _key);

		virtual void mouse_move(int _nx, int _ny);
		virtual void mouse_down(key _btn);
		virtual void mouse_double_click(key _btn);
		virtual void mouse_up(key _btn);

	private:
		void *mInternal;

		delegate<window_event> mResized;
		delegate<window_event> mPainted;
		delegate<window_event> mClosed;

		delegate<key_event> mKeyDown;
		delegate<key_event> mKeyPressed;
		delegate<character_event> mKeyTyped;
		delegate<key_event> mKeyUp;

		delegate<mouse_event> mMouseMove;
		delegate<key_event> mMouseDown;
		delegate<key_event> mMouseDblClick;
		delegate<key_event> mMouseUp;
	};
}
