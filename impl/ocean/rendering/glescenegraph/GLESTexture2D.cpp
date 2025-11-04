/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESTexture2D.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESTexture2D::GLESTexture2D() :
	GLESTexture(),
	Texture2D(),
	DynamicObject()
{
	registerDynamicUpdateObject();
}

GLESTexture2D::~GLESTexture2D()
{
	if (primaryTextureId_ != 0u)
	{
		glDeleteTextures(1, &primaryTextureId_);
		ocean_assert(GL_NO_ERROR == glGetError());
		primaryTextureId_ = 0u;
	}

	if (secondaryTextureId_ != 0u)
	{
		glDeleteTextures(1, &secondaryTextureId_);
		ocean_assert(GL_NO_ERROR == glGetError());
		secondaryTextureId_ = 0u;
	}

	unregisterDynamicUpdateObject();
}

GLESTexture2D::WrapType GLESTexture2D::wrapTypeS() const
{
	const ScopedLock scopedLock(objectLock);

	return wrapTypeS_;
}

GLESTexture2D::WrapType GLESTexture2D::wrapTypeT() const
{
	const ScopedLock scopedLock(objectLock);

	return wrapTypeT_;
}

bool GLESTexture2D::setWrapTypeS(const WrapType type)
{
	const ScopedLock scopedLock(objectLock);

	wrapTypeS_ = type;
	return true;
}

bool GLESTexture2D::setWrapTypeT(const WrapType type)
{
	const ScopedLock scopedLock(objectLock);

	wrapTypeT_ = type;
	return true;
}

std::string GLESTexture2D::textureName() const
{
	const ScopedLock scopedLock(objectLock);

	return textureName_;
}

bool GLESTexture2D::setTextureName(const std::string& name)
{
	if (name.empty())
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	textureName_ = name;

	return true;
}

void GLESTexture2D::createMipmap()
{
	ocean_assert(GL_NO_ERROR == glGetError());

	if (useMipmap_)
	{
		if (primaryTextureId_ != 0u)
		{
			glBindTexture(GL_TEXTURE_2D, primaryTextureId_);
			ocean_assert(GL_NO_ERROR == glGetError());

			glGenerateMipmap(GL_TEXTURE_2D);
			ocean_assert(GL_NO_ERROR == glGetError());
		}

		if (secondaryTextureId_ != 0u)
		{
			glBindTexture(GL_TEXTURE_2D, secondaryTextureId_);
			ocean_assert(GL_NO_ERROR == glGetError());

			glGenerateMipmap(GL_TEXTURE_2D);
			ocean_assert(GL_NO_ERROR == glGetError());
		}
	}

	ocean_assert(GL_NO_ERROR == glGetError());
}

bool GLESTexture2D::defineTextureObject(const FrameType& frameType)
{
	ocean_assert(frameType.isValid());

	return definePrimaryTextureObject(frameType) && defineSecondaryTextureObject(frameType);
}

bool GLESTexture2D::definePrimaryTextureObject(const FrameType& frameType)
{
	ocean_assert(frameType.isValid());

	unsigned int width = 0u, height = 0u;
	GLenum format = 0, type = 0;
	if (!determinePrimaryTextureProperties(frameType, width, height, format, type))
	{
		return false;
	}

	if (primaryTextureId_ == 0u)
	{
		glGenTextures(1, &primaryTextureId_);
		ocean_assert(GL_NO_ERROR == glGetError());

		if (primaryTextureId_ == 0u)
		{
			ocean_assert(false && "This should never happen");
			return false;
		}
	}

	glBindTexture(GL_TEXTURE_2D, primaryTextureId_);
	ocean_assert(GL_NO_ERROR == glGetError());

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	// iOS seems not to support mipmaps with non-power-of-two resolutions

	if (!Utilities::isPowerOfTwo(width) || !Utilities::isPowerOfTwo(height))
	{
		if (minificationFilterMode_ != MIN_MODE_LINEAR)
		{
			Log::warning() << "Due to the non-power-of-two texture the minification filter was set to linear!";
			minificationFilterMode_ = MIN_MODE_LINEAR;
		}

		if (magnificationFilterMode_ != MAG_MODE_LINEAR)
		{
			Log::warning() << "Due to the non-power-of-two texture the magnification filter was set to linear!";
			magnificationFilterMode_ = MAG_MODE_LINEAR;
		}

		if (wrapTypeS_ != WRAP_CLAMP)
		{
			Log::warning() << "Due to the non-power-of-two texture the wrap-s mode was set to clamp-to-edge!";
			wrapTypeS_ = WRAP_CLAMP;
		}

		if (wrapTypeT_ != WRAP_CLAMP)
		{
			Log::warning() << "Due to the non-power-of-two texture the wrap-t mode was set to clamp-to-edge!";
			wrapTypeT_ = WRAP_CLAMP;
		}
	}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, nullptr);
	ocean_assert(GL_NO_ERROR == glGetError());

	return true;
}

bool GLESTexture2D::defineSecondaryTextureObject(const FrameType& frameType)
{
	ocean_assert(frameType.isValid());

	if (!needsSecondaryTextureObjects(frameType))
	{
		return true;
	}

	unsigned int width = 0u, height = 0u;
	GLenum format = 0, type = 0;
	if (!determineSecondaryTextureProperties(frameType, width, height, format, type))
	{
		return false;
	}

	if (secondaryTextureId_ == 0u)
	{
		glGenTextures(1, &secondaryTextureId_);
		ocean_assert(GL_NO_ERROR == glGetError());

		if (secondaryTextureId_ == 0u)
		{
			ocean_assert(false && "This should never happen");
			return false;
		}
	}

	glBindTexture(GL_TEXTURE_2D, secondaryTextureId_);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, nullptr);
	ocean_assert(GL_NO_ERROR == glGetError());

	return true;
}

bool GLESTexture2D::determineAlignment(const unsigned int planeStrideBytes, unsigned int& rowLength, unsigned int& byteAlignment)
{
	ocean_assert(planeStrideBytes >= 1u);

	rowLength = planeStrideBytes;

	if (planeStrideBytes % 4u == 0u)
	{
		byteAlignment = 4u;
	}
	else if (planeStrideBytes % 2u == 0u)
	{
		byteAlignment = 2u;
	}
	else
	{
		byteAlignment = 1u;
	}

	return true;
}

bool GLESTexture2D::needsSecondaryTextureObjects(const FrameType& frameType)
{
	ocean_assert(frameType.isValid());

	switch (frameType.pixelFormat())
	{
		case FrameType::FORMAT_BGR24:
		case FrameType::FORMAT_BGRA32:
		case FrameType::FORMAT_RGB24:
		case FrameType::FORMAT_RGB4444:
		case FrameType::FORMAT_RGB5551:
		case FrameType::FORMAT_RGB565:
		case FrameType::FORMAT_RGBA32:
		case FrameType::FORMAT_RGBA4444:
		case FrameType::FORMAT_YA16:
		case FrameType::FORMAT_Y8:
		case FrameType::FORMAT_Y10_PACKED:
		case FrameType::FORMAT_RGGB10_PACKED:
			return false;

		case FrameType::FORMAT_YUV24:
		case FrameType::FORMAT_YVU24:
			return false;

		case FrameType::FORMAT_Y_UV12_LIMITED_RANGE:
		case FrameType::FORMAT_Y_UV12_FULL_RANGE:
		case FrameType::FORMAT_Y_VU12_LIMITED_RANGE:
		case FrameType::FORMAT_Y_VU12_FULL_RANGE:
		case FrameType::FORMAT_Y_U_V12:
		case FrameType::FORMAT_Y_V_U12:
			return true;

		case FrameType::FORMAT_YUYV16:
			return false;

		default:
			break;
	}

	if (frameType.dataType() == FrameType::DT_SIGNED_FLOAT_32)
	{
		if (frameType.channels() >= 1u && frameType.channels() <= 4u)
		{
			return false;
		}
	}

	ocean_assert(false && "Missing implementation!");
	return false;
}

bool GLESTexture2D::determineInternalFrameType(const FrameType& frameType, FrameType& internalFrameType)
{
	ocean_assert(frameType.isValid());

	switch (frameType.pixelFormat())
	{
		case FrameType::FORMAT_BGR24:
		case FrameType::FORMAT_BGRA32:
		case FrameType::FORMAT_RGB24:
		case FrameType::FORMAT_RGB4444:
		case FrameType::FORMAT_RGB5551:
		case FrameType::FORMAT_RGB565:
		case FrameType::FORMAT_RGBA32:
		case FrameType::FORMAT_RGBA4444:
		case FrameType::FORMAT_YA16:
		case FrameType::FORMAT_Y8:
			internalFrameType = frameType;
			return true;

		case FrameType::FORMAT_Y10_PACKED:
			internalFrameType = FrameType(frameType, FrameType::FORMAT_Y8);
			return true;

		case FrameType::FORMAT_RGGB10_PACKED:
			internalFrameType = FrameType(frameType, FrameType::FORMAT_RGB24);
			return true;

		case FrameType::FORMAT_YUV24:
		case FrameType::FORMAT_YVU24:
		case FrameType::FORMAT_Y_UV12_LIMITED_RANGE:
		case FrameType::FORMAT_Y_UV12_FULL_RANGE:
		case FrameType::FORMAT_Y_VU12_LIMITED_RANGE:
		case FrameType::FORMAT_Y_VU12_FULL_RANGE:
		case FrameType::FORMAT_Y_U_V12:
		case FrameType::FORMAT_Y_V_U12:
			internalFrameType = frameType;
			return true;

		case FrameType::FORMAT_YUYV16:
			internalFrameType = FrameType(frameType, FrameType::FORMAT_RGB24);
			return true;

		default:
			break;
	}

	if (frameType.dataType() == FrameType::DT_SIGNED_FLOAT_32)
	{
		if (frameType.channels() >= 1u && frameType.channels() <= 4u)
		{
			internalFrameType = frameType;
			return true;
		}
	}

	ocean_assert(false && "Missing implementation!");
	return false;
}

bool GLESTexture2D::determinePrimaryTextureProperties(const FrameType& frameType, unsigned int& width, unsigned int& height, GLenum& format, GLenum& type)
{
	ocean_assert(frameType.isValid());

	switch (frameType.pixelFormat())
	{
		case FrameType::FORMAT_BGR24:
			width = frameType.width();
			height = frameType.height();
			format = GL_RGB;
			type = GL_UNSIGNED_BYTE;
			return true;

		case FrameType::FORMAT_BGRA32:
			width = frameType.width();
			height = frameType.height();
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			return true;

		case FrameType::FORMAT_RGB24:
			width = frameType.width();
			height = frameType.height();
			format = GL_RGB;
			type = GL_UNSIGNED_BYTE;
			return true;

		case FrameType::FORMAT_RGB4444:
			width = frameType.width();
			height = frameType.height();
			format = GL_RGB;
			type = GL_UNSIGNED_SHORT_4_4_4_4;
			return true;

		case FrameType::FORMAT_RGB5551:
			width = frameType.width();
			height = frameType.height();
			format = GL_RGB;
			type = GL_UNSIGNED_SHORT_5_5_5_1;
			return true;

		case FrameType::FORMAT_RGB565:
			width = frameType.width();
			height = frameType.height();
			format = GL_RGB;
			type = GL_UNSIGNED_SHORT_5_6_5;
			return true;

		case FrameType::FORMAT_RGBA32:
			width = frameType.width();
			height = frameType.height();
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			return true;

		case FrameType::FORMAT_RGBA4444:
			width = frameType.width();
			height = frameType.height();
			format = GL_RGBA;
			type = GL_UNSIGNED_SHORT_4_4_4_4;
			return true;

		case FrameType::FORMAT_YA16:
			width = frameType.width();
			height = frameType.height();
#ifdef OCEAN_RENDERING_GLES_USE_ES
			format = GL_LUMINANCE_ALPHA;
#else
			format = GL_RG;
#endif
			type = GL_UNSIGNED_BYTE;
			return true;

		case FrameType::FORMAT_Y8:
			width = frameType.width();
			height = frameType.height();
#ifdef OCEAN_RENDERING_GLES_USE_ES
			format = GL_LUMINANCE;
#else
			format = GL_RED;
#endif
			type = GL_UNSIGNED_BYTE;
			return true;

		case FrameType::FORMAT_YUV24:
		case FrameType::FORMAT_YVU24:
			width = frameType.width();
			height = frameType.height();
			format = GL_RGB;
			type = GL_UNSIGNED_BYTE;
			return true;

		case FrameType::FORMAT_Y_UV12_LIMITED_RANGE:
		case FrameType::FORMAT_Y_UV12_FULL_RANGE:
		case FrameType::FORMAT_Y_VU12_LIMITED_RANGE:
		case FrameType::FORMAT_Y_VU12_FULL_RANGE:
		case FrameType::FORMAT_Y_U_V12_LIMITED_RANGE:
		case FrameType::FORMAT_Y_U_V12_FULL_RANGE:
		case FrameType::FORMAT_Y_V_U12_LIMITED_RANGE:
		case FrameType::FORMAT_Y_V_U12_FULL_RANGE:
			width = frameType.width();
			height = frameType.height();
#ifdef OCEAN_RENDERING_GLES_USE_ES
			format = GL_LUMINANCE;
#else
			format = GL_RED;
#endif
			type = GL_UNSIGNED_BYTE;
			return true;

		case FrameType::FORMAT_YUYV16:
			width = frameType.width();
			height = frameType.height();
			format = GL_RGB;
			type = GL_UNSIGNED_BYTE;
			return true;

		default:
			break;
	}

	if (frameType.dataType() == FrameType::DT_SIGNED_FLOAT_32)
	{
		type =  GL_FLOAT;

		switch (frameType.channels())
		{
			case 1u:
				width = frameType.width();
				height = frameType.height();
#ifdef OCEAN_RENDERING_GLES_USE_ES
				format = GL_LUMINANCE;
#else
				format = GL_RED;
#endif
				return true;

			case 2u:
				width = frameType.width();
				height = frameType.height();
#ifdef OCEAN_RENDERING_GLES_USE_ES
				format = GL_LUMINANCE_ALPHA;
#else
				format = GL_RG;
#endif
				return true;

			case 3u:
				width = frameType.width();
				height = frameType.height();
				format = GL_RGB;
				return true;

			case 4u:
				width = frameType.width();
				height = frameType.height();
				format = GL_RGBA;
				return true;
		}
	}

	ocean_assert(false && "Pixel format not supported!");

	width = 0u;
	height = 0u;
	format = 0;
	type = 0;

	return false;
}

bool GLESTexture2D::determineSecondaryTextureProperties(const FrameType& frameType, unsigned int& width, unsigned int& height, GLenum& format, GLenum& type)
{
	ocean_assert(frameType.isValid());

	switch (frameType.pixelFormat())
	{
		case FrameType::FORMAT_BGR24:
		case FrameType::FORMAT_BGRA32:
		case FrameType::FORMAT_RGB24:
		case FrameType::FORMAT_RGB4444:
		case FrameType::FORMAT_RGB5551:
		case FrameType::FORMAT_RGB565:
		case FrameType::FORMAT_RGBA32:
		case FrameType::FORMAT_RGBA4444:
		case FrameType::FORMAT_YA16:
		case FrameType::FORMAT_Y8:
			width = 0u;
			height = 0u;
			format = 0;
			type = 0;
			return false;

		case FrameType::FORMAT_YUV24:
		case FrameType::FORMAT_YVU24:
			width = 0u;
			height = 0u;
			format = 0;
			type = 0;
			return false;

		case FrameType::FORMAT_Y_VU12_LIMITED_RANGE:
		case FrameType::FORMAT_Y_VU12_FULL_RANGE:
		case FrameType::FORMAT_Y_UV12_LIMITED_RANGE:
		case FrameType::FORMAT_Y_UV12_FULL_RANGE:
			width = frameType.width() / 2u;
			height = frameType.height() / 2u;
#ifdef OCEAN_RENDERING_GLES_USE_ES
			format = GL_LUMINANCE_ALPHA;
#else
			format = GL_RG;
#endif
			type = GL_UNSIGNED_BYTE;
			return true;

		case FrameType::FORMAT_Y_U_V12_LIMITED_RANGE:
		case FrameType::FORMAT_Y_U_V12_FULL_RANGE:
		case FrameType::FORMAT_Y_V_U12_LIMITED_RANGE:
		case FrameType::FORMAT_Y_V_U12_FULL_RANGE:
			width = frameType.width() / 2u;
			height = frameType.height();
#ifdef OCEAN_RENDERING_GLES_USE_ES
			format = GL_LUMINANCE;
#else
			format = GL_RED;
#endif
			type = GL_UNSIGNED_BYTE;
			return true;

		case FrameType::FORMAT_YUYV16:
			width = 0u;
			height = 0u;
			format = 0;
			type = 0;
			return false;

		default:
			break;
	}

	ocean_assert(false && "Missing implementation!");
	width = 0u;
	height = 0u;
	format = 0;
	type = 0;
	return false;
}

bool GLESTexture2D::primaryTextureName(const std::string& names, std::string& name)
{
	ocean_assert(!names.empty());
	if (names.empty())
	{
		return false;
	}

	// we accept the following format '<PRIMARY>,<SECONDARY>'

	const std::string::size_type position = names.find(',');

	if (position == std::string::npos)
	{
		name = names;
	}
	else
	{
		name = names.substr(0, position);
	}

	return true;
}

bool GLESTexture2D::secondaryTextureName(const std::string& names, std::string& name)
{
	ocean_assert(!names.empty());
	if (names.empty())
	{
		return false;
	}

	// we accept the following format '<PRIMARY>,<SECONDARY>'

	const std::string::size_type position = names.find(',');

	if (position == std::string::npos || position + 1 == names.size())
	{
		return false;
	}

	name = names.substr(position + 1);
	return true;
}

}

}

}
