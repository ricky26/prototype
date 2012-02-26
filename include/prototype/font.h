#include "prototype/model.h"
#include <netlib/ref_counted.h>

#pragma once

namespace prototype
{
	using netlib::ref_counted;
	using netlib::handle;

	class PROTOTYPE_API font
	{
	public:
		enum flags
		{
			font_bold = (1 << 0),
			font_italic = (1 << 1),
		};

		font();
		font(std::string const& _path, float _size, int _flags=0);
		font(font const& _fn);
		virtual ~font();

		bool valid() const;
		std::string family() const;
		float size() const;
		int flags() const;

		bool create(std::string const& _path, float _size, int _flags=0);

	private:
		void *mInternal;
	};
}