#pragma once

#include "prototype.h"
#include "matrix.h"

#ifndef GLEW_MX
#define GLEW_MX
#endif
#include <GL/glew.h>

PROTOTYPE_API GLEWContext *glewGetContext();

namespace prototype
{
	template<bool C>
	PROTOTYPE_INLINE void glUniformMatrix(GLuint _attr, matrix<4, float, C> const& _mat)
	{
		glUniformMatrix4fv(_attr, 1, C, _mat.get());
	}

	template<bool C>
	PROTOTYPE_INLINE void glUniformMatrix(GLuint _attr, matrix<3, float, C> const& _mat)
	{
		glUniformMatrix3fv(_attr, 1, C, _mat.get());
	}

	template<bool C>
	PROTOTYPE_INLINE void glUniformMatrix(GLuint _attr, matrix<2, float, C> const& _mat)
	{
		glUniformMatrix2fv(_attr, 1, C, _mat.get());
	}
}