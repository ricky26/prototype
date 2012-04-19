#include "texture.h"
#include <netlib/bitstream.h>

#pragma once

namespace prototype
{
	PROTOTYPE_API texture load_png(netlib::bitstream *_strm);
}