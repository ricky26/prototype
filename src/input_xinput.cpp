#include <prototype/input.h>
#include <prototype/binding.h>
#include <prototype/driver_internal.h>
#include <netlib/uthread.h>
#include <sstream>
#include <Windows.h>
#include <Xinput.h>
#define NUM_XINPUT_DEVICES 4

using namespace netlib;

namespace prototype
{
	class xinput_device: public input_device
	{
	public:
		inline void add_control(input_control *_p)
		{
			_p->acquire();
			_p->acquire();
			handle<input_control> hdl(_p);
			input_device::add_control(hdl);
		}

		xinput_device(handle<input_driver> const& _drv, std::string const& _nm)
			: input_device(_drv, _nm),
				LX(this, "Left Stick X-Axis"), LY(this, "Left Stick Y-Axis"),
				LT(this, "Left Trigger"),
				RX(this, "Right Stick X-Axis"), RY(this, "Right Stick Y-Axis"),
				RT(this, "Right Trigger"),
				
				X(this, "X Button"), Y(this, "Y Button"),
				A(this, "A Button"), B(this, "B Button"),
				start(this, "Start Button"), back(this, "Back Button"),
				LB(this, "Left Back Button"), LS(this, "Left Stick Button"),
				RB(this, "Right Back Button"), RS(this, "Right Stick Button"),

				hat(this, "D-Pad")
		{
			add_control(&LX);
			add_control(&LY);
			add_control(&LT);
			add_control(&RX);
			add_control(&RY);
			add_control(&RT);
			
			add_control(&X);
			add_control(&Y);
			add_control(&A);
			add_control(&B);
			add_control(&start);
			add_control(&back);
			
			add_control(&LB);
			add_control(&LS);
			add_control(&RB);
			add_control(&RS);

			add_control(&hat);
		}
		
		linear_input_control LX;
		linear_input_control LY;
		linear_input_control LT;
		linear_input_control RX;
		linear_input_control RY;
		linear_input_control RT;
		
		boolean_input_control X;
		boolean_input_control Y;
		boolean_input_control A;
		boolean_input_control B;
		boolean_input_control start;
		boolean_input_control back;
		
		boolean_input_control LB;
		boolean_input_control LS;
		boolean_input_control RB;
		boolean_input_control RS;

		hat_input_control hat;
	};

	class xinput_driver: public input_driver
	{
	public:
		xinput_driver(): input_driver("xinput")
		{
		}

		bool init() override
		{
			if(!input_driver::init())
				return false;

			enumerate();
			thread = uthread::create(&xinput_driver::think, this);
			return true;
		}

		void think()
		{
			while(thread == uthread::current()
				&& prototype::running())
			{
				enumerate();
				uthread::schedule();
			}

			for(int i = 0; i < NUM_XINPUT_DEVICES; i++)
			{
				if(devices[i])
					remove_device(devices[i]);
			}
		}

		void shutdown() override
		{
			thread = NULL;
			input_driver::shutdown();
		}

		static float short_to_float(short _val)
		{
			if(_val < 0)
				return _val / 32768.f;
			
			return _val / 32767.f;
		}

		void enumerate() override
		{
			for(int i = 0; i < NUM_XINPUT_DEVICES; i++)
			{
				XINPUT_STATE state;
				DWORD ret = XInputGetState(i, &state);
				
				bool conn = (ret != ERROR_DEVICE_NOT_CONNECTED);
				handle<xinput_device> dev = devices[i];

				if(conn && !dev)
				{
					// connected

					std::stringstream ss;
					ss << "XInput Device " << i;
					dev = new xinput_device(this, ss.str());
					devices[i] = dev;
					add_device(dev);
				}
				else if(!conn && dev)
				{
					// disconnected
					devices[i] = NULL;
					remove_device(dev);
					dev->release();
				}

				if(conn && dev)
				{
					// we're still connected,
					// update device state
					
					dev->LX.set_value(short_to_float(state.Gamepad.sThumbLX));
					dev->LY.set_value(short_to_float(state.Gamepad.sThumbLY));
					dev->RX.set_value(short_to_float(state.Gamepad.sThumbRX));
					dev->RY.set_value(short_to_float(state.Gamepad.sThumbRY));

					hat_direction dir = hat_middle;
					if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
						dir = hat_n;
					else if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
						dir = hat_s;
					else if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
						dir = hat_w;
					else if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
						dir = hat_e;
					dev->hat.set_value(dir);
					
					dev->A.set_value((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0);
					dev->B.set_value((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0);
					dev->X.set_value((state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0);
					dev->Y.set_value((state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0);
					
					dev->start.set_value((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0);
					dev->back.set_value((state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0);
					
					dev->LB.set_value((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0);
					dev->LS.set_value((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0);
					dev->RB.set_value((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0);
					dev->RS.set_value((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0);
				}
			}
		}

		handle<xinput_device> devices[NUM_XINPUT_DEVICES];
		handle<uthread> thread;
	};

	static xinput_driver xinput_drv;
	static static_driver_registration gXInputReg(&xinput_drv);
}