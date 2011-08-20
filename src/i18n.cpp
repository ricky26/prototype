#include <prototype/i18n.h>

namespace prototype
{
	namespace i18n
	{
		PROTOTYPE_API std::wstring utf8_to_utf16(std::string const& _text)
		{
			// TODO: Use UCS-2 or something... -- Ricky26

			std::wstring ret;
			ret.resize(_text.size());
			int p = 0;
			for(auto it = _text.begin(); it != _text.end(); it++)
				ret[p++] = *it; // This will _probably_ result in valid UTF16

			return ret;
		}

		PROTOTYPE_API std::string utf16_to_utf8(std::wstring const& _text)
		{
			// TODO: Use UCS-2 or something... -- Ricky26

			std::string ret;
			ret.resize(_text.size());
			int p = 0;
			for(auto it = _text.begin(); it != _text.end(); it++)
				ret[p++] = (char)*it; // This is just plain bad.

			return ret;
		}
	}
}