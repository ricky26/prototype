#include "prototype.h"
#include <string>

#pragma once

namespace prototype
{

	class PROTOTYPE_API mesh
	{
	public:
		struct element
		{
			PROTOTYPE_INLINE element(std::string const& _nm, size_t _stride=sizeof(float), size_t _offset=0, size_t _count=1)
				: name(_nm), stride(_stride), offset(_offset), count(_count) {}

			std::string name;
			size_t stride;
			size_t offset;
			size_t count;
		};

		size_t add_element(element const& _el);
		void remove_element(size_t _idx);
	};
}