/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESStereoAttribute.h"
#include "ocean/rendering/glescenegraph/GLESFramebuffer.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESStereoAttribute::GLESStereoAttribute() :
	GLESAttribute(),
	StereoAttribute(),
	stereoVisibility_(SV_LEFT_AND_RIGHT),
	previousCullFaceValue_(GL_TRUE),
	previousCullFaceMode_(0)
{
	// nothing to do here
}

GLESStereoAttribute::~GLESStereoAttribute()
{
	// nothing to do here
}

GLESStereoAttribute::StereoVisibility GLESStereoAttribute::stereoVisibility() const
{
	return stereoVisibility_;
}

void GLESStereoAttribute::setStereoVisibility(const StereoVisibility stereoVisibility)
{
	stereoVisibility_ = stereoVisibility;
}

void GLESStereoAttribute::bindAttribute(const GLESFramebuffer& framebuffer, GLESShaderProgram& /*shaderProgram*/)
{
	ocean_assert(GL_NO_ERROR == glGetError());

	const bool skipObject = (framebuffer.stereoType() == GLESFramebuffer::ST_LEFT && (stereoVisibility_ & SV_LEFT) == 0)
								|| (framebuffer.stereoType() == GLESFramebuffer::ST_RIGHT && (stereoVisibility_ & SV_RIGHT) == 0);

	ocean_assert(previousCullFaceMode_ == 0);

	if (skipObject)
	{
		previousCullFaceValue_ = GL_TRUE;
		glGetBooleanv(GL_CULL_FACE, &previousCullFaceValue_);

		ocean_assert(GL_NO_ERROR == glGetError());

		glGetIntegerv(GL_CULL_FACE_MODE, &previousCullFaceMode_);
		ocean_assert(previousCullFaceMode_ != 0);

		ocean_assert(GL_NO_ERROR == glGetError());

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT_AND_BACK);
	}
}

void GLESStereoAttribute::unbindAttribute()
{
	ocean_assert(GL_NO_ERROR == glGetError());

	if (previousCullFaceMode_ != 0)
	{
		if (previousCullFaceValue_)
		{
			glEnable(GL_CULL_FACE);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}

		glCullFace(previousCullFaceMode_);

		previousCullFaceMode_ = 0;
	}
}


}

}

}
