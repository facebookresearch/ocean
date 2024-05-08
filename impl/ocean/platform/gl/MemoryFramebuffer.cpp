/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/gl/MemoryFramebuffer.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

MemoryFramebuffer::MemoryFramebuffer() :
	Framebuffer(),
	framebufferObjectId(0u),
	framebufferRenderBufferId(0u),
	framebufferIntermediateObjectId(0u),
	framebufferIntermediateRenderBufferId(0u),
	framebufferIsBound(false),
	framebufferFrame(nullptr)
{
	// nothing to do here
}

MemoryFramebuffer::MemoryFramebuffer(Context& context) :
	Framebuffer(context),
	framebufferObjectId(0u),
	framebufferRenderBufferId(0u),
	framebufferIntermediateObjectId(0u),
	framebufferIntermediateRenderBufferId(0u),
	framebufferIsBound(false),
	framebufferFrame(nullptr)
{
	ocean_assert(associatedContext_);
	associatedContext_->makeCurrent();
}

MemoryFramebuffer::~MemoryFramebuffer()
{
	release();
}

bool MemoryFramebuffer::setContext(Context& context)
{
	if (!Framebuffer::setContext(context))
		return false;

	ocean_assert(associatedContext_);
	associatedContext_->makeCurrent();

	return true;
}

bool MemoryFramebuffer::setTargetFrame(Frame& frame)
{
	ocean_assert(frame.isContinuous());

	if (frame.numberPlanes() != 1u || !frame.isContinuous())
	{
		return false;
	}

	if (frame.width() != framebufferWidth || frame.height() != framebufferHeight || frame.pixelOrigin() != FrameType::ORIGIN_UPPER_LEFT || frame.pixelFormat() != framebufferFormat2pixelFormat(framebufferInternalFormat))
	{
		framebufferFrame = nullptr;
		return false;
	}

	framebufferFrame = &frame;
	return true;
}

bool MemoryFramebuffer::resize(const unsigned int width, const unsigned int height, const GLenum internalFormat)
{
	if (width == framebufferWidth && height == framebufferHeight && internalFormat == framebufferInternalFormat && framebufferObjectId != 0u && framebufferRenderBufferId != 0u)
		return true;

	ocean_assert(associatedContext_);
	if (!associatedContext_)
		return false;

	if (!Framebuffer::resize(width, height, internalFormat))
		return false;

	ocean_assert(framebufferWidth == width);
	ocean_assert(framebufferHeight == height);
	ocean_assert(framebufferInternalFormat == internalFormat);

	ocean_assert(glGetError() == GL_NO_ERROR);

	if (framebufferObjectId == 0u)
	{
		ocean_assert(framebufferRenderBufferId == 0u);

		associatedContext_->glGenFramebuffers(1, &framebufferObjectId);
		ocean_assert(glGetError() == GL_NO_ERROR);

		associatedContext_->glGenRenderbuffers(1, &framebufferRenderBufferId);
		ocean_assert(glGetError() == GL_NO_ERROR);
	}

	ocean_assert(framebufferObjectId != 0u && framebufferRenderBufferId != 0u);

	if (associatedContext_->multisamples() > 1u && framebufferIntermediateObjectId == 0u)
	{
		ocean_assert(framebufferIntermediateRenderBufferId == 0u);

		associatedContext_->glGenFramebuffers(1, &framebufferIntermediateObjectId);
		ocean_assert(glGetError() == GL_NO_ERROR);

		associatedContext_->glGenRenderbuffers(1, &framebufferIntermediateRenderBufferId);
		ocean_assert(glGetError() == GL_NO_ERROR);
	}

	ocean_assert(associatedContext_->multisamples() <= 1u || (framebufferIntermediateObjectId != 0u && framebufferIntermediateRenderBufferId != 0u));

	associatedContext_->glBindRenderbuffer(GL_RENDERBUFFER, framebufferRenderBufferId);
	ocean_assert(glGetError() == GL_NO_ERROR);

	if (associatedContext_->multisamples() <= 1u)
	{
		associatedContext_->glRenderbufferStorage(GL_RENDERBUFFER, framebufferInternalFormat, framebufferWidth, framebufferHeight);
		ocean_assert(glGetError() == GL_NO_ERROR);
	}
	else
	{
		associatedContext_->glRenderbufferStorageMultisample(GL_RENDERBUFFER, associatedContext_->multisamples(), framebufferInternalFormat, framebufferWidth, framebufferHeight);
		ocean_assert(glGetError() == GL_NO_ERROR);

		associatedContext_->glBindRenderbuffer(GL_RENDERBUFFER, framebufferIntermediateRenderBufferId);
		ocean_assert(glGetError() == GL_NO_ERROR);

		associatedContext_->glRenderbufferStorage(GL_RENDERBUFFER, framebufferInternalFormat, framebufferWidth, framebufferHeight);
		ocean_assert(glGetError() == GL_NO_ERROR);
	}

	associatedContext_->glBindRenderbuffer(GL_RENDERBUFFER, 0u);
	ocean_assert(glGetError() == GL_NO_ERROR);

	associatedContext_->glBindFramebuffer(GL_FRAMEBUFFER, framebufferObjectId);
	ocean_assert(glGetError() == GL_NO_ERROR);

	associatedContext_->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, framebufferRenderBufferId);
	ocean_assert(glGetError() == GL_NO_ERROR);

	const GLenum framebufferStatus = associatedContext_->glCheckFramebufferStatus(GL_FRAMEBUFFER);
	ocean_assert(glGetError() == GL_NO_ERROR);

	ocean_assert(framebufferStatus == GL_FRAMEBUFFER_COMPLETE);
	if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		release();
		return false;
	}

	associatedContext_->glBindFramebuffer(GL_FRAMEBUFFER, 0u);
	ocean_assert(glGetError() == GL_NO_ERROR);

	if (framebufferIntermediateObjectId != 0u)
	{
		ocean_assert(framebufferIntermediateRenderBufferId != 0u);

		associatedContext_->glBindFramebuffer(GL_FRAMEBUFFER, framebufferIntermediateObjectId);
		ocean_assert(glGetError() == GL_NO_ERROR);

		associatedContext_->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, framebufferIntermediateRenderBufferId);
		ocean_assert(glGetError() == GL_NO_ERROR);

		const GLenum intermediateFramebufferStatus = associatedContext_->glCheckFramebufferStatus(GL_FRAMEBUFFER);
		ocean_assert(glGetError() == GL_NO_ERROR);

		ocean_assert(intermediateFramebufferStatus == GL_FRAMEBUFFER_COMPLETE);
		if (intermediateFramebufferStatus != GL_FRAMEBUFFER_COMPLETE)
		{
			release();
			return false;
		}

		associatedContext_->glBindFramebuffer(GL_FRAMEBUFFER, 0u);
		ocean_assert(glGetError() == GL_NO_ERROR);
	}

	return true;
}

bool MemoryFramebuffer::release()
{
	if (framebufferObjectId == 0u && framebufferRenderBufferId == 0u && framebufferIntermediateObjectId == 0u && framebufferIntermediateRenderBufferId == 0u)
	{
		ocean_assert(framebufferWidth == 0u);
		ocean_assert(framebufferHeight == 0u);
		ocean_assert(framebufferInternalFormat == 0u);

		return true;
	}

	ocean_assert(associatedContext_);
	if (!associatedContext_)
		return false;

	ocean_assert(glGetError() == GL_NO_ERROR);

	associatedContext_->glDeleteFramebuffers(1, &framebufferObjectId);
	ocean_assert(glGetError() == GL_NO_ERROR);

	associatedContext_->glDeleteRenderbuffers(1, &framebufferRenderBufferId);
	ocean_assert(glGetError() == GL_NO_ERROR);

	associatedContext_->glDeleteFramebuffers(1, &framebufferIntermediateObjectId);
	ocean_assert(glGetError() == GL_NO_ERROR);

	associatedContext_->glDeleteRenderbuffers(1, &framebufferIntermediateRenderBufferId);
	ocean_assert(glGetError() == GL_NO_ERROR);

	framebufferObjectId = 0u;
	framebufferRenderBufferId = 0u;

	framebufferIntermediateObjectId = 0u;
	framebufferIntermediateRenderBufferId = 0u;

	framebufferWidth = 0u;
	framebufferHeight = 0u;
	framebufferInternalFormat = 0u;

	return true;
}

bool MemoryFramebuffer::bind()
{
	if (!associatedContext_)
		return false;

	associatedContext_->glBindFramebuffer(GL_FRAMEBUFFER, framebufferObjectId);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	ocean_assert(glGetError() == GL_NO_ERROR);

	framebufferIsBound = true;

	return true;
}

bool MemoryFramebuffer::unbind()
{
	if (!associatedContext_)
		return false;

	if (framebufferFrame)
	{
		if (associatedContext_->multisamples() <= 1u)
		{
			ocean_assert(framebufferObjectId);
			associatedContext_->glBindFramebuffer(GL_FRAMEBUFFER, framebufferObjectId);
			ocean_assert(glGetError() == GL_NO_ERROR);
		}
		else
		{
			ocean_assert(framebufferObjectId);
			associatedContext_->glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferObjectId);
			ocean_assert(glGetError() == GL_NO_ERROR);

			ocean_assert(framebufferIntermediateObjectId);
			associatedContext_->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferIntermediateObjectId);
			ocean_assert(glGetError() == GL_NO_ERROR);

			associatedContext_->glBlitFramebuffer(0, 0, framebufferWidth, framebufferHeight, 0, 0, framebufferWidth, framebufferHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
			ocean_assert(glGetError() == GL_NO_ERROR);

			ocean_assert(framebufferIntermediateObjectId);
			associatedContext_->glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferIntermediateObjectId);
			ocean_assert(glGetError() == GL_NO_ERROR);
		}

		glReadBuffer(GL_COLOR_ATTACHMENT0);
		ocean_assert(glGetError() == GL_NO_ERROR);

		ocean_assert(framebufferFrame->width() == framebufferWidth && framebufferFrame->height() == framebufferHeight);

		glReadPixels(0, 0, framebufferFrame->width(), framebufferFrame->height(), GL_RGBA, GL_UNSIGNED_BYTE, framebufferFrame->data<void>());
		ocean_assert(glGetError() == GL_NO_ERROR);
	}

	associatedContext_->glBindFramebuffer(GL_FRAMEBUFFER, 0u);
	ocean_assert(glGetError() == GL_NO_ERROR);

	framebufferIsBound = false;

	return true;
}

}

}

}
