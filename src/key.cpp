#include <prototype/key.h>
#include <sstream>

namespace prototype
{
	static std::string gKeyNames[key_num] = {
		"None",

		"Left Button",
		"Right Button",
		"Middle Button",

		"Mouse Button 4",
		"Mouse Button 5",

		"Shift",
		"Right Shift",
		"Control",
		"Right Control",
		"Alt",
		"Right Alt",
		"Meta",
		"Right Meta",

		"Backspace",
		"Tab",
		"Return",
		"Numpad Return",
		"Pause",
		"Capslock",
		"Numlock",
		"Scroll Lock",
		"Escape",
		"Space",

		"Page Up",
		"Page Down",
		"End",
		"Home",
		"Insert",
		"Delete",

		"Left",
		"Right",
		"Up",
		"Down",

		"Print Screen",
		
		"Numpad 0",
		"Numpad 1",
		"Numpad 2",
		"Numpad 3",
		"Numpad 4",
		"Numpad 5",
		"Numpad 6",
		"Numpad 7",
		"Numpad 8",
		"Numpad 9",

		"Numpad *",
		"Numpad /",
		"Numpad -",
		"Numpad +",
		"Numpad Delete",
		
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		
		"F1",
		"F2",
		"F3",
		"F4",
		"F5",
		"F6",
		"F7",
		"F8",
		"F9",
		"F10",
		"F11",
		"F12",
		"F13",
		"F14",
		"F15",
		"F16",
		"F17",
		"F18",
		"F19",
		"F20",
		"F21",
		"F22",
		"F23",
		"F24",
		
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"", // 10 
		"",
		"",
		"",
		"",
		"",

		"A",
		"B",
		"C",
		"D",
		"E",
		"F",
		"G",
		"H",
		"I",
		"J",
		"K",
		"L",
		"M",
		"N",
		"O",
		"P",
		"Q",
		"R",
		"S",
		"T",
		"U",
		"V",
		"W",
		"X",
		"Y",
	};


	PROTOTYPE_API std::string key_name(key const& _key)
	{
		if(_key < key_oem)
		{
			std::string msg = gKeyNames[_key];
			if(msg.length() > 0)
				return msg;
		}

		std::stringstream ss;
		ss << "OEM Key " << _key;
		return ss.str();
	}
}