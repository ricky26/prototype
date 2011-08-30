#include <prototype/input.h>
#include <prototype/binding.h>
#include <prototype/driver_internal.h>
#include <netlib/uthread.h>
#include <Windows.h>

using namespace netlib;

namespace prototype
{
	class mouse_win32: public input_device
	{
	public:
		inline mouse_win32(handle<input_driver> const& _drv): input_device(_drv, "Mouse"),
			mX(this, "X Axis"),
			mY(this, "Y Axis")
		{
			mX.acquire();
			mY.acquire();
		}
		
		relative_input_control mX;
		relative_input_control mY;
	};

	class input_win32: public input_driver
	{
	public:

		static const wchar_t *window_class_name;

		inline input_win32(): input_driver("Windows"), mMouse(this), mWindow(NULL)
		{
		}

		bool init() override
		{
			WNDCLASSEXW cls;
			ZeroMemory(&cls, sizeof(cls));
			
			cls.hInstance = GetModuleHandle(NULL);
			cls.lpszClassName = window_class_name;
			cls.style = CS_OWNDC;
			cls.hCursor = LoadCursor(NULL, IDC_ARROW);
			
			cls.lpfnWndProc = input_win32::window_proc;

			if(!RegisterClassExW(&cls))
				return false;

			mWindow = CreateWindowW(window_class_name, L"InputWindow", 0, CW_DEFAULT, CW_DEFAULT,
				0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);
			if(!mWindow)
				return false;
		
			SetWindowLongPtr(mWindow, GWLP_USERDATA, (LONG)this);

			RAWINPUTDEVICE devs[1];

			devs[0].usUsagePage = 1;
			devs[0].usUsage = 2;
			devs[0].dwFlags = RIDEV_NOLEGACY;
			devs[0].hwndTarget = mWindow;

			if(RegisterRawInputDevices(devs, sizeof(devs)/sizeof(*devs), sizeof(*devs)) == FALSE)
			{
				DestroyWindow(mWindow);
				mWindow = NULL;

				return false;
			}
			
			add_device(&mMouse);
			return true;
		}

		void shutdown() override
		{
			remove_device(&mMouse);

			if(mWindow)
				DestroyWindow(mWindow);
		}

		void enumerate() override
		{
		}

		static LRESULT CALLBACK window_proc(HWND _win,
			UINT _msg, WPARAM _wparam, LPARAM _lparam)
		{
			input_win32 *self = (input_win32*)(GetWindowLongPtr(_win, GWLP_USERDATA));
			
			if(_msg == WM_INPUT)
			{
				UINT size;
				GetRawInputData((HRAWINPUT)_lparam, RID_INPUT,
					NULL, &size, sizeof(RAWINPUTHEADER));

				RAWINPUT *ri = (RAWINPUT*)new uint8_t[size];
				if(!ri)
					return FALSE;

				if(GetRawInputData((HRAWINPUT)_lparam, RID_INPUT,
					ri, &size, sizeof(RAWINPUTHEADER)) != size)
					return FALSE;

				if(ri->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
				{
					self->mMouse.mX.set_value((float)ri->data.mouse.lLastX);
					self->mMouse.mY.set_value((float)ri->data.mouse.lLastY);
				}
				else
				{
					self->mMouse.mX.add_value((float)ri->data.mouse.lLastX);
					self->mMouse.mY.add_value((float)ri->data.mouse.lLastY);
				}

				return 0;
			}
			else if(_msg == WM_INPUT_DEVICE_CHANGE)
			{
			}

			return DefWindowProcW(_win, _msg, _wparam, _lparam);
		}

		mouse_win32 mMouse;
		HWND mWindow;
	};

	const wchar_t *input_win32::window_class_name = L"InputWindow";

	static input_win32 driver;
	static static_driver_registration gIW32Reg(&driver);
}