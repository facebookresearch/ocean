/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/gl/FramebufferObject.h"
#include "ocean/platform/gl/Framebuffer.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

FramebufferObject::FramebufferObject() :
	framebufferObjectId(0u),
	framebufferIsBound(false)
{
	// Nothing to do here.
}

FramebufferObject::~FramebufferObject()
{
	ocean_assert(!framebufferIsBound);

	release();
}

void FramebufferObject::release()
{
	if (framebufferIsBound)
		unbindRenderTarget();

	if (framebufferObjectId != 0u)
	{
		ocean_assert(associatedContext_);

		ocean_assert(glGetError() == GL_NO_ERROR);

		associatedContext_->glDeleteFramebuffers(1, &framebufferObjectId);
		framebufferObjectId = 0u;

		ocean_assert(glGetError() == GL_NO_ERROR);
	}
}

void FramebufferObject::bindRenderTarget(Texture& texture)
{
	ocean_assert(associatedContext_);
	if (associatedContext_ && !framebufferObjectId)
	{
		associatedContext_->glGenFramebuffers(1, &framebufferObjectId);
		ocean_assert(glGetError() == GL_NO_ERROR);
	}

	ocean_assert(framebufferObjectId);
	if (!framebufferObjectId)
		return;

	ocean_assert(!framebufferIsBound);

	associatedContext_->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferObjectId);
	ocean_assert(glGetError() == GL_NO_ERROR);

	associatedContext_->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.id(), 0);
	ocean_assert(glGetError() == GL_NO_ERROR);

	const GLenum value = associatedContext_->glCheckFramebufferStatus(GL_FRAMEBUFFER);
	ocean_assert(glGetError() == GL_NO_ERROR && value == GL_FRAMEBUFFER_COMPLETE);

	if (value != GL_FRAMEBUFFER_COMPLETE)
		return;

	glGetIntegerv(GL_VIEWPORT, framebufferOldViewportCoordinates);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glViewport(0, 0, texture.frameType().width(), texture.frameType().height());
	ocean_assert(glGetError() == GL_NO_ERROR);

	framebufferIsBound = true;
}

void FramebufferObject::unbindRenderTarget()
{
	ocean_assert(associatedContext_);
	ocean_assert(framebufferIsBound);
	ocean_assert(framebufferObjectId);

	if (!associatedContext_)
		return;

	associatedContext_->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0u);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glViewport(framebufferOldViewportCoordinates[0], framebufferOldViewportCoordinates[1], framebufferOldViewportCoordinates[2], framebufferOldViewportCoordinates[3]);
	ocean_assert(glGetError() == GL_NO_ERROR);

	framebufferIsBound = false;
}

}

}

}
