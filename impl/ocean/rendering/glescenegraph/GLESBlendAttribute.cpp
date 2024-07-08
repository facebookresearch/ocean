/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESBlendAttribute.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESBlendAttribute::GLESBlendAttribute() :
	GLESAttribute(),
	BlendAttribute(),
	attributeSourceBlendingFunction(FUNCTION_SOURCE_ALPHA),
	attributeDestinationBlendingFunction(FUNCTION_ONE_MINUS_SOURCE_ALPHA),
	attributeGLESSourceBlendingFunction(GL_SRC_ALPHA),
	attributeGLESDestinationBlendingFunction(GL_ONE_MINUS_SRC_ALPHA)
{
	// nothing to do here
}

GLESBlendAttribute::~GLESBlendAttribute()
{
	// nothing to do here
}

GLESBlendAttribute::BlendingFunction GLESBlendAttribute::sourceFunction() const
{
	return attributeSourceBlendingFunction;
}

GLESBlendAttribute::BlendingFunction GLESBlendAttribute::destinationFunction() const
{
	return attributeDestinationBlendingFunction;
}

void GLESBlendAttribute::setSourceFunction(const BlendingFunction function)
{
	if (function == attributeSourceBlendingFunction)
		return;

	attributeSourceBlendingFunction = function;
	attributeGLESSourceBlendingFunction = translateSourceBlendingFunction(function);
}

void GLESBlendAttribute::setDestinationFunction(const BlendingFunction function)
{
	if (function == attributeDestinationBlendingFunction)
		return;

	attributeDestinationBlendingFunction = function;
	attributeGLESDestinationBlendingFunction = translateDestinationBlendingFunction(function);
}

void GLESBlendAttribute::bindAttribute(const GLESFramebuffer& /*framebuffer*/, GLESShaderProgram& /*shaderProgram*/)
{
	glEnable(GL_BLEND);
	ocean_assert(GL_NO_ERROR == glGetError());

	glBlendFunc(attributeGLESSourceBlendingFunction, attributeGLESDestinationBlendingFunction);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESBlendAttribute::unbindAttribute()
{
	glDisable(GL_BLEND);
	ocean_assert(GL_NO_ERROR == glGetError());
}

GLenum GLESBlendAttribute::translateSourceBlendingFunction(const BlendingFunction function)
{
	switch (function)
	{
		case FUNCTION_ZERO:
			return GL_ZERO;

		case FUNCTION_ONE:
			return GL_ONE;

		case FUNCTION_SOURCE_COLOR:
			return GL_SRC_COLOR;

		case FUNCTION_ONE_MINUS_SOURCE_COLOR:
			return GL_ONE_MINUS_SRC_COLOR;

		case FUNCTION_SOURCE_ALPHA:
			return GL_SRC_ALPHA;

		case FUNCTION_ONE_MINUS_SOURCE_ALPHA:
			return GL_ONE_MINUS_SRC_ALPHA;

		case FUNCTION_DESTINATION_ALPHA:
			return GL_DST_ALPHA;

		case FUNCTION_ONE_MINUS_DESTINATION_ALPHA:
			return GL_ONE_MINUS_DST_ALPHA;

		default:
			break;
	}

	throw OceanException("Invalid source blending function.");
}

GLenum GLESBlendAttribute::translateDestinationBlendingFunction(const BlendingFunction function)
{
	switch (function)
	{
		case FUNCTION_ZERO:
			return GL_ZERO;

		case FUNCTION_ONE:
			return GL_ONE;

		case FUNCTION_DESTINATION_COLOR:
			return GL_DST_COLOR;

		case FUNCTION_ONE_MINUS_DESTINATION_COLOR:
			return GL_ONE_MINUS_DST_COLOR;

		case FUNCTION_SOURCE_ALPHA:
			return GL_SRC_ALPHA;

		case FUNCTION_ONE_MINUS_SOURCE_ALPHA:
			return GL_ONE_MINUS_SRC_ALPHA;

		case FUNCTION_DESTINATION_ALPHA:
			return GL_DST_ALPHA;

		case FUNCTION_ONE_MINUS_DESTINATION_ALPHA:
			return GL_ONE_MINUS_DST_ALPHA;

		case FUNCTION_SOURCE_ALPHA_SATURATE:
			return GL_SRC_ALPHA_SATURATE;

		default:
			break;
	}

	throw OceanException("Invalid destination blending function.");
}

}

}

}
