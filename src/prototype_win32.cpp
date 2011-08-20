#include <prototype/prototype.h>
#include <prototype/window.h>
#include <prototype/driver.h>
#include <netlib/netlib.h>
#include <netlib/thread.h>
#include <Windows.h>

using namespace netlib;

namespace prototype
{
	static _declspec(thread) uint64_t time_base = 0;
	static _declspec(thread) uint64_t tick_base = 0;
	static _declspec(thread) uint64_t cpu_freq = 0;
	static _declspec(thread) uint64_t the_time = 0;
	static handle<thread> main_thread;

	static inline void query_cpu()
	{
		QueryPerformanceFrequency((LARGE_INTEGER*)&cpu_freq);
		QueryPerformanceCounter((LARGE_INTEGER*)&time_base);
	}

	static inline bool update_time()
	{
		if(cpu_freq <= 0) return false;

		uint64_t count = 0;
		if(QueryPerformanceCounter((LARGE_INTEGER*)&count) != TRUE)
			return false;

		if(count < time_base)
			return false;

		count -= time_base;
		the_time = (count * 1000000) / cpu_freq;
		return true;
	}

	PROTOTYPE_API bool init()
	{
		if(!netlib::init())
			return false;

		if(!window::init())
			return false;

		if(!driver::register_static_drivers())
			return false;
		
		timeBeginPeriod(1);

		tick_base = GetTickCount64();
		query_cpu();

		main_thread = thread::current();
		return true;
	}

	PROTOTYPE_API void shutdown()
	{
		window::shutdown();

		timeEndPeriod(1);
	}

	PROTOTYPE_API void exit(int _ret)
	{
		netlib::exit(_ret);
	}

	PROTOTYPE_API int exit_value()
	{
		return netlib::exit_value();
	}

	PROTOTYPE_API bool think()
	{
		if(!netlib::think())
			return false;

		if(thread::current() == main_thread)
		{
			if(!update_time())
				return false;
		}

		return true;
	}

	PROTOTYPE_API int run_main_loop()
	{
		while(think());
		shutdown();
		return exit_value();
	}
}