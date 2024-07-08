/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/gl/TextureFramebuffer.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

TextureFramebuffer::TextureFramebuffer(Context& context) :
	Framebuffer(context)
{
	// nothing to do here
}

bool TextureFramebuffer::setSamples(const unsigned int samples)
{
	ocean_assert(samples >= 1u);

	if (samples_ == max(1u, samples))
		return true;

	ocean_assert(associatedContext_);
	if (!associatedContext_)
		return false;

	const unsigned int oldWidth = framebufferWidth;
	const unsigned int oldHeight = framebufferHeight;
	const unsigned int oldInternalFormat = framebufferInternalFormat;

	release();

	ocean_assert(framebufferWidth == 0u);
	ocean_assert(framebufferHeight == 0u);
	ocean_assert(framebufferInternalFormat == 0u);

	samples_ = max(1u, samples);

	if (oldWidth == 0u || oldHeight == 0u)
		return true;

	return resize(oldWidth, oldHeight, oldInternalFormat);
}

bool TextureFramebuffer::resize(const unsigned int width, const unsigned int height, const GLenum internalFormat)
{
	if (width == framebufferWidth && height == framebufferHeight && framebufferInternalFormat == internalFormat && objectId_ != 0u && textureId_ != 0u)
		return true;

	ocean_assert(associatedContext_);
	if (!associatedContext_)
		return false;

	associatedContext_->makeCurrent();

	ocean_assert(glGetError() == GL_NO_ERROR);

	if (objectId_ == 0u)
	{
		associatedContext_->glGenFramebuffers(1, &objectId_);
		ocean_assert(glGetError() == GL_NO_ERROR);
	}

	ocean_assert(objectId_ != 0u);

	associatedContext_->glBindFramebuffer(GL_FRAMEBUFFER, objectId_);
	ocean_assert(glGetError() == GL_NO_ERROR);

	if (textureId_ == 0u)
	{
		glGenTextures(1, &textureId_);
		ocean_assert(glGetError() == GL_NO_ERROR);
	}

	ocean_assert(textureId_ != 0u);

	if (samples_ == 1u)
	{
		glBindTexture(GL_TEXTURE_2D, textureId_);
		ocean_assert(glGetError() == GL_NO_ERROR);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		ocean_assert(glGetError() == GL_NO_ERROR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		ocean_assert(glGetError() == GL_NO_ERROR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		ocean_assert(glGetError() == GL_NO_ERROR);

		glBindTexture(GL_TEXTURE_2D, 0u);
		ocean_assert(glGetError() == GL_NO_ERROR);

		associatedContext_->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId_, 0);
		ocean_assert(glGetError() == GL_NO_ERROR);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureId_);
		ocean_assert(glGetError() == GL_NO_ERROR);

		associatedContext_->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples_, internalFormat, width, height, GL_TRUE);
		ocean_assert(glGetError() == GL_NO_ERROR);

		glBindTexture(GL_TEXTURE_2D, 0u);
		ocean_assert(glGetError() == GL_NO_ERROR);

		associatedContext_->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureId_, 0);
		ocean_assert(glGetError() == GL_NO_ERROR);
	}

	const GLenum value = associatedContext_->glCheckFramebufferStatus(GL_FRAMEBUFFER);
	ocean_assert(glGetError() == GL_NO_ERROR);

	ocean_assert(value == GL_FRAMEBUFFER_COMPLETE);
	if (value != GL_FRAMEBUFFER_COMPLETE)
	{
		release();
		return false;
	}

	if (!Framebuffer::resize(width, height, internalFormat))
	{
		release();
		return false;
	}

	return true;
}

bool TextureFramebuffer::bind()
{
	ocean_assert(objectId_ != 0u);
	if (objectId_ == 0u)
		return false;

	associatedContext_->glBindFramebuffer(GL_FRAMEBUFFER, objectId_);
	ocean_assert(glGetError() == GL_NO_ERROR);

	return true;
}

bool TextureFramebuffer::bindToProgram(const GLuint programId, const std::string& attributeName, const unsigned int index)
{
	ocean_assert(associatedContext_);
	if (!associatedContext_)
		return false;

	ocean_assert(GL_NO_ERROR == glGetError());

	ocean_assert(textureId_ != 0u);
	ocean_assert(programId != 0u);
	ocean_assert(index < GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
	ocean_assert(!attributeName.empty());

	ocean_assert(associatedContext_->glIsProgram(programId));
	associatedContext_->glUseProgram(programId);
	ocean_assert(GL_NO_ERROR == glGetError());

	associatedContext_->glActiveTexture(GLenum(GL_TEXTURE0 + index));
	ocean_assert(GL_NO_ERROR == glGetError());

	if (samples_ == 1u)
	{
		glBindTexture(GL_TEXTURE_2D, textureId_);
		ocean_assert(GL_NO_ERROR == glGetError());
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureId_);
		ocean_assert(GL_NO_ERROR == glGetError());
	}

	const GLint location = associatedContext_->glGetUniformLocation(programId, attributeName.c_str());
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());

	associatedContext_->glUniform1i(location, GLint(index));
	ocean_assert(GL_NO_ERROR == glGetError());

	return true;
}

bool TextureFramebuffer::unbind()
{
	associatedContext_->glBindFramebuffer(GL_FRAMEBUFFER, 0u);
	return true;
}

bool TextureFramebuffer::release()
{
	if (textureId_ != 0u)
	{
		ocean_assert(glGetError() == GL_NO_ERROR);

		glDeleteTextures(1, &textureId_);
		textureId_ = 0u;

		ocean_assert(glGetError() == GL_NO_ERROR);
	}

	if (objectId_ != 0u)
	{
		ocean_assert(associatedContext_);
		if (!associatedContext_)
			return false;

		ocean_assert(glGetError() == GL_NO_ERROR);

		associatedContext_->glDeleteFramebuffers(1, &objectId_);
		objectId_ = 0u;

		ocean_assert(glGetError() == GL_NO_ERROR);
	}

	framebufferWidth = 0u;
	framebufferHeight = 0u;
	framebufferInternalFormat = 0u;

	return true;
}

}

}

}
