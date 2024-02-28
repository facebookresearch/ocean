// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
