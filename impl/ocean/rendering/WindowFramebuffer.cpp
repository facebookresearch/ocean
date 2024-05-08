/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/WindowFramebuffer.h"

namespace Ocean
{

namespace Rendering
{

WindowFramebuffer::WindowFramebuffer() :
	Framebuffer()
{
	// nothing to do here
}

WindowFramebuffer::~WindowFramebuffer()
{
	// nothing to do here
}

WindowFramebuffer::FramebufferType WindowFramebuffer::framebufferType() const
{
	return FRAMEBUFFER_WINDOW;
}

size_t WindowFramebuffer::id() const
{
	throw NotSupportedException();
}

bool WindowFramebuffer::initializeById(const size_t /*id*/, const FramebufferRef& /*shareFramebuffer*/)
{
	throw NotSupportedException();
}

WindowFramebuffer::ObjectType WindowFramebuffer::type() const
{
	return TYPE_WINDOW_FRAMEBUFFER;
}

}

}
