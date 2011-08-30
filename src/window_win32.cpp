#include <prototype/window.h>
#include <prototype/i18n.h>
#include <prototype/gl.h>
#include <prototype/key.h>
#include <netlib/ref_counted.h>
#include <netlib/thread.h>
#include <Windows.h>
#include <Windowsx.h>

using namespace netlib;

namespace prototype
{
	static const wchar_t window_class_name[] = L"PrototypeWindow";
	static GLEWContext *glew_context = NULL;
	static const LONG_PTR windowed_style = WS_OVERLAPPEDWINDOW;
	static const LONG_PTR fullscreen_style = WS_POPUP;

	static const key key_map[256] = {
		key_none,
		key_lbutton,	// 0x01
		key_rbutton,	// 0x02
		key_none,
		key_mbutton,
		key_xbutton,
		key_ybutton,
		key_none,		// 0x07
		key_backspace,	
		key_tab,
		key_none, // 10
		key_none,
		key_none, // clear
		key_return,
		key_none,
		key_none,
		key_shift,
		key_control,
		key_alt,
		key_pause,
		key_caps, // 20
		key_none, // kana
		key_none, 
		key_none, // junja
		key_none, // ime final
		key_none, // kanji
		key_none,
		key_escape,
		key_none, // ime convert
		key_none,
		key_none, // ime mode change // 30
		key_none,
		key_space, 
		key_pgup,
		key_pgdown,
		key_end,
		key_home,
		key_left,
		key_up,
		key_right,
		key_down,  // 40
		key_none,
		key_none,
		key_none,
		key_printscrn,
		key_insert,
		key_delete,
		key_none,
		key_0,
		key_1,
		key_2,
		key_3,
		key_4,
		key_5,
		key_6,
		key_7,
		key_8,
		key_9,
		key_none, // 0x3a
		key_none,
		key_none,
		key_none,
		key_none,
		key_none,
		key_none,
		key_a,
		key_b,
		key_c,
		key_d,
		key_e,
		key_f,
		key_g,
		key_h,
		key_i,
		key_j,
		key_k,
		key_l,
		key_m,
		key_n,
		key_o,
		key_p,
		key_q,
		key_r,
		key_s,
		key_t,
		key_u,
		key_v,
		key_w,
		key_x,
		key_y,
		key_z,
		key_meta,
		key_right_meta,
		key_none,
		key_none,
		key_none, // sleep
		key_numpad_0,
		key_numpad_1,
		key_numpad_2,
		key_numpad_3,
		key_numpad_4,
		key_numpad_5,
		key_numpad_6,
		key_numpad_7,
		key_numpad_8,
		key_numpad_9,
		key_numpad_multiply,
		key_numpad_add,
		key_none,
		key_numpad_subtract,
		key_numpad_delete,
		key_numpad_divide,
		key_f1,
		key_f2,
		key_f3,
		key_f4,
		key_f5,
		key_f6,
		key_f7,
		key_f8,
		key_f9,
		key_f10,
		key_f11,
		key_f13,
		key_f14,
		key_f15,
		key_f16,
		key_f17,
		key_f18,
		key_f19,
		key_f20,
		key_f21,
		key_f22,
		key_f23,
		key_f24,
		key_numlock,
		key_scrollock,
		key_none,
		key_none,
		key_none,
		key_none,
		key_none,
		key_shift,
		key_right_shift,
		key_control,
		key_right_control,
		key_alt,
		key_right_alt,
	};

	static key map_key(uint16_t _key)
	{
		if(_key > VK_SCROLL)
			return (key)(key_oem + (_key - VK_SCROLL));

		return key_map[_key];
	}

	//
	// window_internal
	//

	struct window_internal: public ref_counted
	{
		HWND handle;
		HDC dc;
		HGLRC rc;
		GLEWContext context;
		bool fullscreen;
		DEVMODEW mode;
		RECT rect;
		int modifiers;
		HCURSOR cursor;
		bool active;

		window_internal()
		{
			handle = NULL;
			dc = NULL;
			rc = NULL;
			cursor = LoadCursor(NULL, IDC_ARROW);
			fullscreen = false;
			active = true;
			modifiers = 0;
		}

		~window_internal()
		{
			if(rc)
			{
				wglDeleteContext(rc);
				rc = NULL;
			}

			if(dc)
			{
				ReleaseDC(handle, dc);
				dc = NULL;
			}

			if(handle)
			{
				DestroyWindow(handle);
				handle = NULL;
			}
		}

		inline bool setup_dc()
		{
			if(dc)
				return true;

			if(!handle)
				return false;

			dc = GetDC(handle);
			if(!dc)
				return false;

			PIXELFORMATDESCRIPTOR pfd;
			memset(&pfd, 0, sizeof(pfd));

			pfd.nSize = sizeof(pfd);
			pfd.nVersion = 1;
			pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cColorBits = 24;
			pfd.cDepthBits = 16;
			pfd.iLayerType = PFD_MAIN_PLANE;

			int fmt = ChoosePixelFormat(dc, &pfd);
			SetPixelFormat(dc, fmt, &pfd);
			return true;
		}

		inline bool setup_rc()
		{
			if(rc)
				return true;

			rc = wglCreateContext(dc);
			if(!rc)
				return false;

			static HGLRC last_rc = NULL;
			static netlib::critical_section cs;
			cs.lock();

			if(last_rc)
				wglShareLists(last_rc, rc);

			last_rc = rc;
			cs.unlock();

			if(glewContextInit(&context) != GL_TRUE)
				return false;

			return true;
		}

		static LRESULT CALLBACK window_proc(HWND _win,
			UINT _msg, WPARAM _wparam, LPARAM _lparam)
		{
			window *win = (window*)GetWindowLongPtr(_win, GWLP_USERDATA);
			if(win)
			{
				window_internal *wi = get(win->mInternal);

				switch(_msg)
				{
				case WM_ACTIVATE:
					wi->active = LOWORD(_wparam) != 0;
					break;

				case WM_PAINT:
					{
						ValidateRect(_win, NULL);
						win->paint();
					}
					return 0;

				case WM_SETCURSOR:
					win->show_cursor();
					break;

				case WM_KILLFOCUS:
					wi->modifiers = 0;
					if(wi->fullscreen)
					{
						ChangeDisplaySettingsW(NULL, 0);
						ShowWindow(wi->handle, SW_MINIMIZE);
					}
					break;

				case WM_SETFOCUS:
					if(wi->fullscreen)
						ChangeDisplaySettingsW(&wi->mode, CDS_FULLSCREEN);
					break;

				case WM_SIZE:
					{
						RedrawWindow(wi->handle, NULL, NULL, RDW_INTERNALPAINT);
						window_event evt(win);
						win->on_resized()(evt);
					}
					return 0;

				case WM_MOUSEMOVE:
					win->mouse_move(GET_X_LPARAM(_lparam), GET_Y_LPARAM(_lparam));
					return 0;

				case WM_LBUTTONDOWN:
					win->mouse_down(key_lbutton);
					return 0;

				case WM_LBUTTONUP:
					win->mouse_up(key_lbutton);
					return 0;

				case WM_LBUTTONDBLCLK:
					win->mouse_double_click(key_lbutton);
					return 0;

				case WM_RBUTTONDOWN:
					win->mouse_down(key_rbutton);
					return 0;

				case WM_RBUTTONUP:
					win->mouse_up(key_rbutton);
					return 0;

				case WM_RBUTTONDBLCLK:
					win->mouse_double_click(key_rbutton);
					return 0;

				case WM_MBUTTONDOWN:
					win->mouse_down(key_mbutton);
					return 0;

				case WM_MBUTTONUP:
					win->mouse_up(key_mbutton);
					return 0;

				case WM_MBUTTONDBLCLK:
					win->mouse_double_click(key_mbutton);
					return 0;

				case WM_XBUTTONDOWN:
					{
						key btn = (key)(key_xbutton+GET_XBUTTON_WPARAM(_wparam)-1);
						win->mouse_down(btn);
					}
					return 0;

				case WM_XBUTTONUP:
					{
						key btn = (key)(key_xbutton+GET_XBUTTON_WPARAM(_wparam)-1);
						win->mouse_up(btn);
					}
					return 0;

				case WM_XBUTTONDBLCLK:
					{
						key btn = (key)(key_xbutton+GET_XBUTTON_WPARAM(_wparam)-1);
						win->mouse_double_click(btn);
					}
					return 0;

				case WM_SYSKEYDOWN:
				case WM_KEYDOWN:
					{
						int btn = _wparam;
						//bool was_down = ((1 << 30) & _lparam) != 0;

						if(btn == VK_SHIFT)
							wi->modifiers |= modifier_shift;
						else if(btn == VK_MENU)
							wi->modifiers |= modifier_alt;
						else if(btn == VK_CONTROL)
							wi->modifiers |= modifier_control;

						btn = map_key(btn);
						if(btn && _lparam & (1 << 24))
							btn++;

						if(btn)
						{
							//if(!was_down)
								win->key_down((key)btn);

							win->key_pressed((key)btn);
						}
					}
					return 0;
					
				case WM_SYSKEYUP:
				case WM_KEYUP:
					{
						int btn = _wparam;

						if(btn == VK_SHIFT)
							wi->modifiers &=~ modifier_shift;
						else if(btn == VK_MENU)
							wi->modifiers &=~ modifier_alt;
						else if(btn == VK_CONTROL)
							wi->modifiers &=~ modifier_control;
						
						btn = map_key(btn);
						if(btn && _lparam & (1 << 24))
							btn++;

						if(btn)
							win->key_up((key)btn);
					}
					return 0;

				case WM_CHAR:
					win->key_typed(_wparam);
					return 0;
				}
			}

			return DefWindowProcW(_win, _msg, _wparam, _lparam);
		}

		static inline window_internal *get(void *_val)
		{
			return static_cast<window_internal*>(_val);
		}
	};

	//
	// window
	//
	
	window::window()
	{
		window_internal *wi = new window_internal();
		wi->acquire();
		mInternal = wi;
	}

	window::window(std::string const& _title)
	{
		window_internal *wi = new window_internal();
		wi->acquire();
		mInternal = wi;

		create(_title);
	}

	window::~window()
	{
		window_internal *wi = window_internal::get(mInternal);
		wi->release();
		mInternal = NULL;
	}

	bool window::valid() const
	{
		window_internal *wi = window_internal::get(mInternal);
		return wi->handle != NULL;
	}

	bool window::create(std::string const& _title)
	{
		window_internal *wi = window_internal::get(mInternal);
		if(valid())
			return false;

		std::wstring wtitle = i18n::utf8_to_utf16(_title);
		HWND win = CreateWindowW(window_class_name, wtitle.c_str(), windowed_style,
			CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);
		if(!win)
			return false;

		SetWindowLongPtr(win, GWLP_USERDATA, (LONG)this);
		wi->handle = win;
		return true;
	}

	std::string window::title() const
	{
		if(!valid())
			return std::string();
		
		window_internal *wi = window_internal::get(mInternal);
		std::wstring wstr;

		int len = GetWindowTextLengthW(wi->handle);
		wstr.resize(len);

		len = GetWindowTextW(wi->handle, (LPWSTR)wstr.data(), wstr.size());
		wstr.resize(len);

		return i18n::convert<i18n::utf8, i18n::utf16>(wstr);
	}

	void window::set_title(std::string const& _title)
	{
		if(!valid())
			return;
		
		window_internal *wi = window_internal::get(mInternal);
		std::wstring wstr = i18n::utf8_to_utf16(_title);
		SetWindowTextW(wi->handle, wstr.c_str());
	}

	int window::width() const
	{
		if(!valid())
			return 0;
		
		window_internal *wi = window_internal::get(mInternal);

		RECT rect;
		if(GetWindowRect(wi->handle, &rect) == FALSE)
			return 0;

		return rect.right - rect.left;
	}

	int window::height() const
	{
		if(!valid())
			return 0;
		
		window_internal *wi = window_internal::get(mInternal);

		RECT rect;
		if(GetWindowRect(wi->handle, &rect) == FALSE)
			return 0;

		return rect.bottom - rect.top;
	}

	void window::set_size(int _sx, int _sy)
	{
		if(!valid())
			return;
		
		window_internal *wi = window_internal::get(mInternal);
		SetWindowPos(wi->handle, NULL, 0, 0, _sx, _sy, SWP_NOMOVE | SWP_NOZORDER);
	}

	bool window::active() const
	{
		window_internal *wi = window_internal::get(mInternal);
		return wi->active;
	}

	void window::activate()
	{
		window_internal *wi = window_internal::get(mInternal);
		SetActiveWindow(wi->handle);
	}

	bool window::visible() const
	{		
		window_internal *wi = window_internal::get(mInternal);
		return IsWindowVisible(wi->handle) == TRUE;
	}

	void window::set_visible(bool _v)
	{
		window_internal *wi = window_internal::get(mInternal);
		ShowWindow(wi->handle, _v ? SW_SHOW : SW_HIDE);
	}
	
		
	bool window::cursor_visible() const
	{
		window_internal *wi = window_internal::get(mInternal);
		return wi->cursor != NULL;
	}

	void window::set_cursor_visible(bool _v)
	{
		window_internal *wi = window_internal::get(mInternal);
		wi->cursor = _v ? LoadCursor(NULL, IDC_ARROW) : NULL;
		SetCursor(wi->cursor);
	}
	
	bool window::fullscreen(int _w, int _h, int _r)
	{
		window_internal *wi = window_internal::get(mInternal);
		if(wi->fullscreen)
			return true; // Already fullscreen

		// Get current mode
		DEVMODEW *mode = &wi->mode;
		if(EnumDisplaySettingsW(NULL, ENUM_CURRENT_SETTINGS, mode) != TRUE)
			return false;

		mode->dmPelsWidth = _w;
		mode->dmPelsHeight = _h;
		mode->dmDisplayFrequency = _r;
		mode->dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

		// Test new mode
		if(ChangeDisplaySettingsW(mode, CDS_TEST) != DISP_CHANGE_SUCCESSFUL)
		{
			mode->dmFields &=~ DM_DISPLAYFREQUENCY;
			if(ChangeDisplaySettingsW(mode, CDS_TEST) != DISP_CHANGE_SUCCESSFUL)
				return false;
		}

		// Change style
		if(GetWindowRect(wi->handle, &wi->rect) != TRUE)
			return false;
		
		LONG_PTR style = GetWindowLongPtr(wi->handle, GWL_STYLE);
		style = (style &~ windowed_style) | fullscreen_style;
		SetWindowLongPtr(wi->handle, GWL_STYLE, style);

		SetWindowPos(wi->handle, HWND_TOPMOST,
			0, 0, _w, _h, SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowWindow(wi->handle, SW_MAXIMIZE);

		// Finally change window mode.
		LONG ret = ChangeDisplaySettingsW(mode, CDS_FULLSCREEN);
		wi->fullscreen = (ret == DISP_CHANGE_SUCCESSFUL);

		return wi->fullscreen;
	}

	void window::minimise()
	{
		window_internal *wi = window_internal::get(mInternal);
		if(wi->fullscreen)
			restore();

		ShowWindow(wi->handle, SW_MINIMIZE);
	}

	void window::maximise()
	{
		window_internal *wi = window_internal::get(mInternal);
		if(wi->fullscreen)
			restore();

		ShowWindow(wi->handle, SW_MAXIMIZE);
	}

	void window::restore()
	{
		window_internal *wi = window_internal::get(mInternal);
		if(wi->fullscreen)
		{
			// Fullscreen. Fix it.
			ChangeDisplaySettings(NULL, 0);
			
			LONG_PTR style = GetWindowLongPtrW(wi->handle, GWL_STYLE);
			style = (style &~ fullscreen_style) | windowed_style;
			SetWindowLongPtrW(wi->handle, GWL_STYLE, style);

			int w = wi->rect.right - wi->rect.left;
			int h = wi->rect.bottom - wi->rect.top;
			ShowWindow(wi->handle, SW_RESTORE);
			SetWindowPos(wi->handle, HWND_TOP, wi->rect.left, wi->rect.top, w, h,
				SWP_FRAMECHANGED);

			wi->fullscreen = false;
			return;
		}

		ShowWindow(wi->handle, SW_RESTORE);
	}

	void window::close()
	{
		window_internal *wi = window_internal::get(mInternal);
		if(wi->fullscreen)
		{
			// Fullscreen. Fix it.
			ChangeDisplaySettings(NULL, 0);
			
			LONG_PTR style = GetWindowLongPtrW(wi->handle, GWL_STYLE);
			style = (style &~ fullscreen_style) | windowed_style;
			SetWindowLongPtrW(wi->handle, GWL_STYLE, style);

			SetWindowPos(wi->handle, HWND_TOP, 0, 0, 0, 0,
				SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

			wi->fullscreen = false;
		}

		DestroyWindow(wi->handle);
		wi->handle = NULL;
	}

	bool window::begin_frame()
	{
		window_internal *wi = window_internal::get(mInternal);
		if(!wi->setup_dc() || !wi->setup_rc())
			return false;

		wglMakeCurrent(wi->dc, wi->rc);
		glew_context = &wi->context;
		return true;
	}

	void window::end_frame()
	{
		window_internal *wi = window_internal::get(mInternal);
		if(wi->dc)
			SwapBuffers(wi->dc);
	}

	bool window::grab_mouse()
	{
		window_internal *wi = window_internal::get(mInternal);
		SetCapture(wi->handle);

		RECT rect;
		POINT tl,br;
		GetClientRect(wi->handle, &rect);
		
		tl.x = rect.left;
		tl.y = rect.top;
		ClientToScreen(wi->handle, &tl);

		br.x = rect.right;
		br.y = rect.bottom;
		ClientToScreen(wi->handle, &br);

		rect.left = tl.x;
		rect.top = tl.y;
		rect.right = br.x;
		rect.bottom = br.y;
		ClipCursor(&rect);
		return true;
	}

	void window::release_mouse()
	{
		ClipCursor(NULL);
		ReleaseCapture();
	}

	void window::repaint()
	{
		window_internal *wi = window_internal::get(mInternal);
		RedrawWindow(wi->handle, NULL, NULL, RDW_INTERNALPAINT);
	}

	void window::paint()
	{
		window_event evt(this);
		mPainted(evt);
	}

	void window::key_down(key _key)
	{
		key_event evt(this, _key, true);
		mKeyDown(evt);
	}

	void window::key_pressed(key _key)
	{
		key_event evt(this, _key, true);
		mKeyPressed(evt);
	}

	void window::key_typed(wchar_t _chr)
	{
		character_event evt(this, _chr);
		mKeyTyped(evt);
	}

	void window::key_up(key _key)
	{
		key_event evt(this, _key, false);
		mKeyUp(evt);
	}

	void window::mouse_move(int _nx, int _ny)
	{
		mouse_event evt(this, _nx, _ny);
		mMouseMove(evt);
	}

	void window::mouse_down(key _btn)
	{
		key_event evt(this, _btn, true);
		mMouseDown(evt);
	}

	void window::mouse_double_click(key _btn)
	{
		key_event evt(this, _btn, true);
		mMouseDblClick(evt);
	}

	void window::mouse_up(key _btn)
	{
		key_event evt(this, _btn, false);
		mMouseUp(evt);
	}

	bool window::init()
	{
		WNDCLASSEX windowClass;
		memset(&windowClass, 0, sizeof(windowClass));
		windowClass.cbSize = sizeof(windowClass);
			
		windowClass.hInstance = GetModuleHandle(NULL);
		windowClass.lpszClassName = window_class_name;
		windowClass.style = CS_OWNDC;
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
			
		windowClass.lpfnWndProc = window_internal::window_proc;

		if(!RegisterClassEx(&windowClass))
			return false;

		return true;
	}

	void window::think()
	{
	}

	void window::shutdown()
	{
	}
}

PROTOTYPE_API GLEWContext *glewGetContext()
{
	return prototype::glew_context;
}
