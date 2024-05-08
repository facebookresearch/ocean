/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESDepthAttribute.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESDepthAttribute::GLESDepthAttribute() :
	GLESAttribute(),
	DepthAttribute(),
	testingEnabled_(true),
	writingEnabled_(true),
	depthFunction_(FUNCTION_LESS),
	glesDepthFunction(GL_LESS)
{
	// nothing to do here
}

GLESDepthAttribute::~GLESDepthAttribute()
{
	// nothing to do here
}

bool GLESDepthAttribute::testingEnabled() const
{
	return testingEnabled_;
}

bool GLESDepthAttribute::writingEnabled() const
{
	return writingEnabled_;
}

GLESDepthAttribute::DepthFunction GLESDepthAttribute::depthFunction() const
{
	return depthFunction_;
}

void GLESDepthAttribute::setTestingEnabled(const bool state)
{
	testingEnabled_ = state;
}

void GLESDepthAttribute::setWritingEnabled(const bool state)
{
	writingEnabled_ = state;
}

void GLESDepthAttribute::setDepthFunction(const DepthFunction function)
{
	if (function == depthFunction_)
	{
		return;
	}

	depthFunction_ = function;
	glesDepthFunction = translateDepthFunction(function);
}

void GLESDepthAttribute::bindAttribute(const GLESFramebuffer& /*framebuffer*/, GLESShaderProgram& /*shaderProgram*/)
{
	ocean_assert(GL_NO_ERROR == glGetError());

	if (testingEnabled_)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	ocean_assert(GL_NO_ERROR == glGetError());

	glDepthFunc(glesDepthFunction);
	ocean_assert(GL_NO_ERROR == glGetError());

	glDepthMask(writingEnabled_ ? GL_TRUE : GL_FALSE);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESDepthAttribute::unbindAttribute()
{
	ocean_assert(GL_NO_ERROR == glGetError());

	glEnable(GL_DEPTH_TEST);
	ocean_assert(GL_NO_ERROR == glGetError());

	glDepthFunc(GL_LESS);
	ocean_assert(GL_NO_ERROR == glGetError());

	glDepthMask(GL_TRUE);
	ocean_assert(GL_NO_ERROR == glGetError());
}

GLenum GLESDepthAttribute::translateDepthFunction(const DepthFunction function)
{
	switch (function)
	{
		case FUNCTION_ALWAYS:
			return GL_ALWAYS;

		case FUNCTION_EQUAL:
			return GL_EQUAL;

		case FUNCTION_GREATER:
			return GL_GREATER;

		case FUNCTION_GREATER_EQUAL:
			return GL_GEQUAL;

		case FUNCTION_LESS:
			return GL_LESS;

		case FUNCTION_LESS_EQUAL:
			return GL_LEQUAL;

		case FUNCTION_NEVER:
			return GL_NEVER;

		case FUNCTION_NOT_EQUAL:
			return GL_NOTEQUAL;
	}

	ocean_assert(false && "Invalid depth function.");
	throw OceanException("Invalid depth function.");
}

}

}

}
