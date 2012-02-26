#include <prototype/i18n.h>
#include <Windows.h>

namespace prototype
{
	namespace i18n
	{
		PROTOTYPE_API std::wstring utf8_to_utf16(std::string const& _text)
		{
			std::wstring ret;
			ret.resize(_text.size());

			if(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
				_text.data(), _text.size(), (wchar_t*)ret.data(), ret.size()) == 0)
				return L""; // TODO: check actual string size?
			
			return ret;
		}

		PROTOTYPE_API std::string utf16_to_utf8(std::wstring const& _text)
		{
			std::string ret;
			ret.resize(_text.size()*2);
			
			if(WideCharToMultiByte(CP_UTF8, MB_ERR_INVALID_CHARS,
				_text.data(), _text.size(), (char*)ret.data(), ret.size(),
				NULL, FALSE) == 0)
				return ""; // TODO: check actual string size?

			return ret;
		}
	}
}