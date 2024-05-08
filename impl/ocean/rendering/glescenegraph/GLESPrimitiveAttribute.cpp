/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESPrimitiveAttribute.h"
#include "ocean/rendering/glescenegraph/GLESFramebuffer.h"
#include "ocean/rendering/glescenegraph/GLESShaderProgram.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESPrimitiveAttribute::GLESPrimitiveAttribute() :
	GLESAttribute(),
	PrimitiveAttribute(),
	faceMode_(MODE_DEFAULT),
	cullingMode_(CULLING_BACK),
	glesCullingMode_(GL_BACK),
	lightingMode_(LM_SINGLE_SIDE_LIGHTING),
	unbindNecessary_(false)
{
	// nothing to do here
}

GLESPrimitiveAttribute::~GLESPrimitiveAttribute()
{
	// nothing to do here
}

GLESPrimitiveAttribute::FaceMode GLESPrimitiveAttribute::faceMode() const
{
	return faceMode_;
}

GLESPrimitiveAttribute::CullingMode GLESPrimitiveAttribute::cullingMode() const
{
	return cullingMode_;
}

GLESPrimitiveAttribute::LightingMode GLESPrimitiveAttribute::lightingMode() const
{
	return lightingMode_;
}

void GLESPrimitiveAttribute::setFaceMode(const FaceMode mode)
{
	switch (mode)
	{
		case MODE_FACE:
		case MODE_POINT:
		case MODE_DEFAULT:
			faceMode_ = mode;
			break;

		case MODE_LINE:
			throw NotSupportedException("MODE_LINE is not supported in OpenGL ES");
	}
}

void GLESPrimitiveAttribute::setCullingMode(const CullingMode culling)
{
	switch (culling)
	{
		case CULLING_BACK:
			glesCullingMode_ = GL_BACK;
			break;

		case CULLING_FRONT:
			glesCullingMode_ = GL_FRONT;
			break;

		case CULLING_BOTH:
			glesCullingMode_ = GL_FRONT_AND_BACK;
			break;

		default:
			ocean_assert(culling == CULLING_NONE);
			break;
	};

	cullingMode_ = culling;
}

void GLESPrimitiveAttribute::setLightingMode(const LightingMode lighting)
{
	lightingMode_ = lighting;
}

void GLESPrimitiveAttribute::bindAttribute(const GLESFramebuffer& framebuffer, GLESShaderProgram& shaderProgram)
{
	if (framebuffer.lightingMode() == LM_DEFAULT)
	{
		const GLint locationLightingTwoSided = glGetUniformLocation(shaderProgram.id(), "lightingTwoSided");
		if (locationLightingTwoSided != -1)
		{
			setUniform(locationLightingTwoSided, lightingMode_ == LM_TWO_SIDED_LIGHTING ? 1 : 0);
		}
	}

	if (framebuffer.cullingMode() == CULLING_DEFAULT)
	{
		if (cullingMode_ == CULLING_NONE)
		{
			ocean_assert(GL_NO_ERROR == glGetError());
			glDisable(GL_CULL_FACE);
			ocean_assert(GL_NO_ERROR == glGetError());
		}
		else
		{
			ocean_assert(GL_NO_ERROR == glGetError());
			glCullFace(glesCullingMode_);
			ocean_assert(GL_NO_ERROR == glGetError());
		}

		unbindNecessary_ = cullingMode_ != CULLING_BACK;
	}
}

void GLESPrimitiveAttribute::unbindAttribute()
{
	if (unbindNecessary_)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		ocean_assert(GL_NO_ERROR == glGetError());
	}
}

}

}

}
