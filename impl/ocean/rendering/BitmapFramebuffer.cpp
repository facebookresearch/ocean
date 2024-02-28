// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/BitmapFramebuffer.h"

namespace Ocean
{

namespace Rendering
{

BitmapFramebuffer::BitmapFramebuffer() :
	Framebuffer()
{
	// nothing to do here
}

BitmapFramebuffer::~BitmapFramebuffer()
{
	// nothing to do here
}

bool BitmapFramebuffer::initialize(const FramebufferRef& /*shareFramebuffer*/)
{
	return true;
}

BitmapFramebuffer::FramebufferType BitmapFramebuffer::framebufferType() const
{
	return FRAMEBUFFER_BITMAP;
}

BitmapFramebuffer::ObjectType BitmapFramebuffer::type() const
{
	return TYPE_BITMAP_FRAMEBUFFER;
}

}

}
