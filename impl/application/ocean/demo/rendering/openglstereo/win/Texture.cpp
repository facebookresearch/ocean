/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/rendering/openglstereo/win/Texture.h"

using namespace Ocean;
using namespace Ocean::Media;

Texture::~Texture()
{
	release();
}

FrameType Texture::update()
{
	if (frameMedium_.isNull())
	{
		if (textureId_ != 0)
		{
			glDeleteTextures(1, &textureId_);
			ocean_assert(glGetError() == GL_NO_ERROR);

			textureId_ = 0;
		}

		return FrameType();
	}

	const FrameRef frame = frameMedium_->frame();
	if (frame.isNull())
	{
		return FrameType();
	}

	if (frameType_ != frame->frameType())
	{
		texturePixelFormat_ = 0;

		switch (frame->pixelFormat())
		{
			case FrameType::FORMAT_BGR24:
				texturePixelFormat_ = GL_BGR_EXT;
				break;

			case FrameType::FORMAT_BGRA32:
				texturePixelFormat_ = GL_BGRA_EXT;
				break;

			case FrameType::FORMAT_RGB24:
				texturePixelFormat_ = GL_RGB;
				break;

			case FrameType::FORMAT_RGBA32:
				texturePixelFormat_ = GL_RGBA;
				break;

			case FrameType::FORMAT_Y8:
			case FrameType::FORMAT_Y_U_V12:
			case FrameType::FORMAT_Y_V_U12:
				texturePixelFormat_ = GL_LUMINANCE;
				break;

			case FrameType::FORMAT_YA16:
				texturePixelFormat_ = GL_LUMINANCE_ALPHA;
				break;

			default:
				return FrameType();
		}

		if (textureId_ == 0)
		{
			glGenTextures(1, &textureId_);
			ocean_assert(glGetError() == GL_NO_ERROR);

			if (textureId_ == 0)
				return FrameType();

			glBindTexture(GL_TEXTURE_2D, textureId_);
			ocean_assert(glGetError() == GL_NO_ERROR);

			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			ocean_assert(glGetError() == GL_NO_ERROR);

			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			ocean_assert(glGetError() == GL_NO_ERROR);
		}

		ocean_assert(frame->isContinuous());

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame->width(), frame->height(), 0, texturePixelFormat_, GL_UNSIGNED_BYTE, frame->constdata<void>());
		ocean_assert(glGetError() == GL_NO_ERROR);

		frameType_ = frame->frameType();
		frameTimestamp_ = frame->timestamp();
	}
	else
	{
		if (frame->timestamp() > frameTimestamp_)
		{
			glBindTexture(GL_TEXTURE_2D, textureId_);
			ocean_assert(glGetError() == GL_NO_ERROR);

			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame->width(), frame->height(), texturePixelFormat_, GL_UNSIGNED_BYTE, frame->constdata<void>());
			ocean_assert(glGetError() == GL_NO_ERROR);

			frameTimestamp_ = frame->timestamp();
		}
	}

	return frameType_;
}

void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, textureId_);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::release()
{
	if (textureId_ != 0)
	{
		glDeleteTextures(1, &textureId_);
		ocean_assert(glGetError() == GL_NO_ERROR);

		textureId_  = 0;
	}
}

FrameMediumRef Texture::medium()
{
	return frameMedium_;
}

void Texture::setMedium(const FrameMediumRef& medium)
{
	frameTimestamp_ = 0;

	frameMedium_ = medium;
}
