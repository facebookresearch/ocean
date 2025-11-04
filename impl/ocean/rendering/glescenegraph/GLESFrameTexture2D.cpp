/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESFrameTexture2D.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESFrameTexture2D::GLESFrameTexture2D() :
	GLESTexture(),
	Texture2D(),
	DynamicObject(),
	GLESTexture2D(),
	FrameTexture2D()
{
	// nothing to do here
}

GLESFrameTexture2D::~GLESFrameTexture2D()
{
	// nothing to do here
}

bool GLESFrameTexture2D::setTexture(Frame&& frame)
{
	ocean_assert(frame.isValid());
	if (!frame.isValid())
	{
		return false;
	}

	FrameType internalFrameType;
	if (!determineInternalFrameType(frame.frameType(), internalFrameType))
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	frame_ = std::move(frame);
	compressedFrame_.release();

	updateNeeded_ = true;

	return true;
}

bool GLESFrameTexture2D::setTexture(CompressedFrame&& compressedFrame)
{
	ocean_assert(compressedFrame.isValid());
	if (!compressedFrame.isValid())
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	compressedFrame_ = std::move(compressedFrame);
	frame_.release();

	updateNeeded_ = true;

	return true;
}

FrameType GLESFrameTexture2D::frameType() const
{
	const ScopedLock scopedLock(objectLock);

	ocean_assert(!frame_.isValid() || !compressedFrame_.isValid());

	if (frame_.isValid())
	{
		return frame_.frameType();
	}

	return compressedFrame_.internalFrameType();
}

bool GLESFrameTexture2D::hasTransparentPixel() const
{
	const ScopedLock scopedLock(objectLock);

	if (frame_.isValid())
	{
		if (frame_.hasTransparentPixel<uint8_t>(0xFF))
		{
			return true;
		}
	}
	else if (compressedFrame_.internalFrameType().isValid() && FrameType::formatHasAlphaChannel(compressedFrame_.internalFrameType().pixelFormat()))
	{
		return true;
	}

	return false;
}

bool GLESFrameTexture2D::isValid() const
{
	const ScopedLock scopedLock(objectLock);

	return primaryTextureId_ != 0u;
}

void GLESFrameTexture2D::onDynamicUpdate(const ViewRef& /*view*/, const Timestamp /*timestamp*/)
{
	const ScopedLock scopedLock(objectLock);

	if (updateNeeded_)
	{
		ocean_assert(GL_NO_ERROR == glGetError());

		if (frame_.isValid())
		{
			if (!GLESTexture2D::updateTexture(frame_))
			{
				ocean_assert(false && "Failed to update texture!");
				return;
			}
		}
		else
		{
			ocean_assert(compressedFrame_.isValid());

			updateTexture(compressedFrame_);
		}

		updateNeeded_ = false;
	}
}

bool GLESFrameTexture2D::updateTexture(const CompressedFrame& compressedFrame)
{
	ocean_assert(compressedFrame_.isValid());

	if (primaryTextureId_ == 0u)
	{
		glGenTextures(1, &primaryTextureId_);
		ocean_assert(GL_NO_ERROR == glGetError());
	}

	glBindTexture(GL_TEXTURE_2D, primaryTextureId_);
	ocean_assert(GL_NO_ERROR == glGetError());

	GLenum internalFormat = 0;
	if (!determineCompressedFormat(compressedFrame_.compressedFormat(), internalFormat))
	{
		ocean_assert(false && "This must never happen!");
		return false;
	}

	frameType_ = compressedFrame_.internalFrameType();

	unsigned int width = compressedFrame_.internalFrameType().width();
	unsigned int height = compressedFrame_.internalFrameType().height();

	size_t remainingBufferSize = compressedFrame_.buffer().size();
	const uint8_t* buffer = compressedFrame_.buffer().data();

	for (unsigned int nLevel = 0u; nLevel < compressedFrame_.mipmapLevels(); ++nLevel)
	{
		unsigned int levelSize = 0u;
		if (!compressedImageSize(compressedFrame_.compressedFormat(), width, height, levelSize) || size_t(levelSize) > remainingBufferSize)
		{
			Log::error() << "Failed to create compressed texture for level " << nLevel;
			break;
		}

		glCompressedTexImage2D(GL_TEXTURE_2D, GLint(nLevel), internalFormat, GLsizei(width), GLsizei(height), 0, GLsizei(levelSize), buffer);

		const GLenum error = glGetError();

		if (error != GL_NO_ERROR)
		{
			Log::error() << "Failed to specify 2D compressed texture for level " << nLevel << ", error: " << int(error);
			break;
		}

		if (width == 1u && height == 1u)
		{
			break;
		}

		width = std::max(1u, width / 2u);
		height = std::max(1u, height / 2u);

		ocean_assert(remainingBufferSize >= size_t(levelSize));
		remainingBufferSize -= size_t(levelSize);

		buffer += levelSize;
	}

	if (useMipmap_)
	{
		createMipmap();
	}

	return true;
}

bool GLESFrameTexture2D::determineCompressedFormat(const CompressedFormat compressedFormat, GLenum& internalFormat)
{
	switch (compressedFormat)
	{
		case CF_INVALID:
			return false;

		case CF_RGBA_ASTC_4X4:
			internalFormat = 0x93B0; // GL_COMPRESSED_RGBA_ASTC_4x4;
			return true;

		case CF_RGBA_ASTC_6X6:
			internalFormat = 0x93B4; // GL_COMPRESSED_RGBA_ASTC_6x6;
			return true;

		case CF_RGBA_ASTC_8X8:
			internalFormat = 0x93B7; // GL_COMPRESSED_RGBA_ASTC_8x8;
			return true;

		case CF_SRGBA_ASTC_4X4:
			internalFormat = 0x93D0; // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4;
			return true;

		case CF_SRGBA_ASTC_6X6:
			internalFormat = 0x93D4; // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6;
			return true;

		case CF_SRGBA_ASTC_8X8:
			internalFormat = 0x93D7; // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8;
			return true;
	}

	return false;
}

bool GLESFrameTexture2D::compressedImageSize(const CompressedFormat compressedFormat, const unsigned int width, const unsigned int height, unsigned int& size)
{
	ocean_assert(width != 0u && height != 0u);
	if (width == 0u || height == 0u)
	{
		return false;
	}

	unsigned int multiple = 0u;

	switch (compressedFormat)
	{
		case CF_INVALID:
			break;

		case CF_RGBA_ASTC_4X4:
		case CF_SRGBA_ASTC_4X4:
			multiple = 4u;
			break;

		case CF_RGBA_ASTC_6X6:
		case CF_SRGBA_ASTC_6X6:
			multiple = 6u;
			break;

		case CF_RGBA_ASTC_8X8:
		case CF_SRGBA_ASTC_8X8:
			multiple = 8u;
			break;
	}

	if (multiple == 0u)
	{
		ocean_assert(false && "Invalid format!");
		return false;
	}

	size = (unsigned int)(NumericF::ceil(float(width) / float(multiple)) * NumericF::ceil(float(height) / float(multiple))) * 16u;

	return true;
}

}

}

}
