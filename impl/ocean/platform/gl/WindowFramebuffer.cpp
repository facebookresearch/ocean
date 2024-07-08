/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/gl/WindowFramebuffer.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

WindowFramebuffer::WindowFramebuffer() :
	Framebuffer()
{
	// nothing to do here
}

WindowFramebuffer::WindowFramebuffer(Context& context) :
	Framebuffer(context)
{
	// nothing to do here
}

bool WindowFramebuffer::bind()
{
	ocean_assert(glGetError() == GL_NO_ERROR);
	glDrawBuffer(GL_BACK);
	ocean_assert(glGetError() == GL_NO_ERROR);

	return true;
}

bool WindowFramebuffer::unbind()
{
	if (!associatedContext_)
		return false;

	associatedContext_->swapBuffers();
	return true;
}

}

}

}
