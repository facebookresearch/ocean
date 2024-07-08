/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/gl/Framebuffer.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

Framebuffer::~Framebuffer()
{
	// nothing to do here
}

bool Framebuffer::resize(const unsigned int width, const unsigned int height, const GLenum internalFormat)
{
	if (!associatedContext_)
		return false;

	associatedContext_->makeCurrent();

	ocean_assert(glGetError() == GL_NO_ERROR);
	glViewport(0, 0, int(width), int(height));
	ocean_assert(glGetError() == GL_NO_ERROR);

	framebufferWidth = width;
	framebufferHeight = height;
	framebufferInternalFormat = internalFormat;

	return true;
}

bool Framebuffer::initOpenGL()
{
	if (!associatedContext_)
		return false;

	// some of the following configurations may not be applied for non-fixed-function-pipelines

	ocean_assert(glGetError() == GL_NO_ERROR);

	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glEnable(GL_TEXTURE_2D);
	const GLenum glEnableTexture = glGetError();
	ocean_assert_and_suppress_unused(glEnableTexture == GL_NO_ERROR || glEnableTexture == GL_INVALID_ENUM, glEnableTexture);

	glEnable(GL_BLEND);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glClearDepth(1.0f);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glDepthFunc(GL_LEQUAL);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glEnable(GL_ALPHA_TEST);
	const GLenum glEnableAlpha = glGetError();
	ocean_assert_and_suppress_unused(glEnableAlpha == GL_NO_ERROR || glEnableAlpha == GL_INVALID_ENUM, glEnableAlpha);

	glEnable(GL_BLEND);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glEnable(GL_DEPTH_TEST);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glShadeModel(GL_SMOOTH);
	const GLenum glEnableSmooth = glGetError();
	ocean_assert_and_suppress_unused(glEnableSmooth == GL_NO_ERROR || glEnableSmooth == GL_INVALID_OPERATION, glEnableSmooth);

	glEnable(GL_LIGHTING);
	const GLenum glEnableLighting = glGetError();
	ocean_assert_and_suppress_unused(glEnableLighting == GL_NO_ERROR || glEnableLighting == GL_INVALID_ENUM, glEnableLighting);

	glEnable(GL_LIGHT0);
	const GLenum glEnableLight0 = glGetError();
	ocean_assert_and_suppress_unused(glEnableLight0 == GL_NO_ERROR || glEnableLight0 == GL_INVALID_ENUM, glEnableLight0);

	glEnable(GL_COLOR_MATERIAL);
	const GLenum glEnableMaterial = glGetError();
	ocean_assert_and_suppress_unused(glEnableMaterial == GL_NO_ERROR || glEnableMaterial == GL_INVALID_ENUM, glEnableMaterial);

	glEnable(GL_NORMALIZE);
	const GLenum glEnableNormalize = glGetError();
	ocean_assert_and_suppress_unused(glEnableNormalize == GL_NO_ERROR || glEnableNormalize == GL_INVALID_ENUM, glEnableNormalize);

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	const GLenum glEnablePerspective = glGetError();
	ocean_assert_and_suppress_unused(glEnablePerspective == GL_NO_ERROR || glEnablePerspective == GL_INVALID_ENUM, glEnablePerspective);

	return true;
}

FrameType::PixelFormat Framebuffer::framebufferFormat2pixelFormat(const GLenum framebufferFormat)
{
	switch (framebufferFormat)
	{
		case GL_RGBA8:
			return FrameType::FORMAT_RGBA32;
	}

	ocean_assert(false && "Not supported framebuffer format!");
	return FrameType::FORMAT_UNDEFINED;
}

}

}

}
