/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESTexture2D.h"
#include "ocean/rendering/glescenegraph/GLESAttributeSet.h"

#include "ocean/cv/FrameConverter.h"

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

unsigned int GLESTexture2D::bindTexture(GLESShaderProgram& shaderProgram, const unsigned int id)
{
	ocean_assert(GL_NO_ERROR == glGetError());

	const ScopedLock scopedLock(objectLock);

	if (primaryTextureId_ == 0u)
	{
		return 0u;
	}

	const GLenum glesMinificationFilterMode = translateMinificationFilterMode(minificationFilterMode_);
	const GLenum glesMagnificationFilterMode = translateMagnificationFilterMode(magnificationFilterMode_);

	const GLenum glesWrapTypeS = translateWrapType(wrapTypeS_);
	const GLenum glesWrapTypeT = translateWrapType(wrapTypeT_);

	glActiveTexture(GLenum(GL_TEXTURE0 + id));
	ocean_assert(GL_NO_ERROR == glGetError());

	glBindTexture(GL_TEXTURE_2D, primaryTextureId_);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glesMinificationFilterMode);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glesMagnificationFilterMode);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glesWrapTypeS);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glesWrapTypeT);
	ocean_assert(GL_NO_ERROR == glGetError());

	const GLint locationTextureTransformMatrix = glGetUniformLocation(shaderProgram.id(), "textureTransformationMatrix");
	if (locationTextureTransformMatrix != -1)
	{
		ocean_assert(transformation_.isValid());
		setUniform(locationTextureTransformMatrix, transformation_);
	}

	const GLint locationTextureOriginLowerLeft = glGetUniformLocation(shaderProgram.id(), "textureOriginLowerLeft");
	if (locationTextureOriginLowerLeft != -1)
	{
		setUniform(locationTextureOriginLowerLeft, frameType_.pixelOrigin() == FrameType::ORIGIN_LOWER_LEFT ? 1 : 0);
	}

	std::string primaryTexture;
	if (primaryTextureName(textureName_, primaryTexture))
	{
		const GLint locationTexture = glGetUniformLocation(shaderProgram.id(), primaryTexture.c_str());
		if (locationTexture != -1)
		{
			setUniform(locationTexture, int(id));
		}
	}

	if (secondaryTextureId_ == 0u)
	{
		return 1u;
	}

	glActiveTexture(GLenum(GL_TEXTURE0 + id + 1u));
	ocean_assert(GL_NO_ERROR == glGetError());

	glBindTexture(GL_TEXTURE_2D, secondaryTextureId_);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glesMinificationFilterMode);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glesMagnificationFilterMode);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glesWrapTypeS);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glesWrapTypeT);
	ocean_assert(GL_NO_ERROR == glGetError());

	std::string secondaryTexture;
	if (secondaryTextureName(textureName_, secondaryTexture))
	{
		const GLint locationSecondaryTexture = glGetUniformLocation(shaderProgram.id(), secondaryTexture.c_str());

		if (locationSecondaryTexture != -1)
		{
			setUniform(locationSecondaryTexture, int(id + 1u));

			return 2u;
		}
		else
		{
			ocean_assert(false && "This should never happen!");
		}
	}
	else
	{
		ocean_assert(false && "This should never happen!");
	}

	return 1u;
}

bool GLESTexture2D::updateTexture(const Frame& frame)
{
	ocean_assert(frame.isValid());

	FrameType internalFrameType;
	if (!determineInternalFrameType(frame, internalFrameType))
	{
		return false;
	}

	ocean_assert(internalFrameType.isValid());

	if (internalFrameType != frameType_)
	{
		if (!defineTextureObject(internalFrameType))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		const bool resetShaderProgram = frameType_.isValid();

		frameType_ = internalFrameType;

		if (resetShaderProgram)
		{
			// the pixel format has changed, we need to use a new shader

			for (const ObjectRef& texturesObject : parentObjects())
			{
				for (const ObjectRef& attributeSetObject : texturesObject->parentObjects())
				{
					attributeSetObject.force<GLESAttributeSet>().resetShaderProgram();
				}
			}
		}
	}

	frameTimestamp_ = frame.timestamp();

	GLenum format = 0u;
	GLenum type = 0u;
	unsigned int width = 0u;
	unsigned int height = 0u;

	if (!determinePrimaryTextureProperties(frameType_, width, height, format, type))
	{
		return false;
	}

	const Frame* primaryTextureFrame = &frame;
	bool mayNeedSecondaryTexture = true;

	if (frame.frameType() != frameType_)
	{
		CV::FrameConverter::Options convertOptions;
		if (frame.pixelFormat() == FrameType::FORMAT_Y10_PACKED || frame.pixelFormat() == FrameType::FORMAT_RGGB10_PACKED)
		{
			ocean_assert(frameType_.pixelFormat() == FrameType::FORMAT_Y8 || frameType_.pixelFormat() == FrameType::FORMAT_RGB24);

			constexpr float gamma = 0.7f;
			convertOptions = CV::FrameConverter::Options(gamma);
		}

		if (!CV::FrameConverter::Comfort::convert(frame, frameType_.pixelFormat(), frameType_.pixelOrigin(), conversionFrame_, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, nullptr, convertOptions))
		{
			return false;
		}

		ocean_assert(conversionFrame_.numberPlanes() == 1u);

		primaryTextureFrame = &conversionFrame_;
		mayNeedSecondaryTexture = false;
	}

	ocean_assert(primaryTextureId_ != 0u);
	ocean_assert(GL_NO_ERROR == glGetError());

	glBindTexture(GL_TEXTURE_2D, primaryTextureId_);
	ocean_assert(GL_NO_ERROR == glGetError());

	ocean_assert(primaryTextureFrame != nullptr);
	ocean_assert(primaryTextureFrame->dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	unsigned int rowLength = 0u;
	unsigned int byteAlignment = 0u;
	if (!determineAlignment(primaryTextureFrame->strideBytes(0u), rowLength, byteAlignment))
	{
		return false;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, int(byteAlignment));
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, primaryTextureFrame->constdata<void>(0u));
	ocean_assert(GL_NO_ERROR == glGetError());

	if (mayNeedSecondaryTexture && determineSecondaryTextureProperties(frameType_, width, height, format, type))
	{
		ocean_assert(secondaryTextureId_ != 0u);
		ocean_assert(GL_NO_ERROR == glGetError());

		glBindTexture(GL_TEXTURE_2D, secondaryTextureId_);
		ocean_assert(GL_NO_ERROR == glGetError());

		switch (frameType_.pixelFormat())
		{
			case FrameType::FORMAT_Y_VU12_LIMITED_RANGE:
			case FrameType::FORMAT_Y_VU12_FULL_RANGE:
			case FrameType::FORMAT_Y_UV12_LIMITED_RANGE:
			case FrameType::FORMAT_Y_UV12_FULL_RANGE:
			{
				rowLength = 0u;
				byteAlignment = 0u;
				if (!determineAlignment(frame.strideBytes(1u), rowLength, byteAlignment))
				{
					return false;
				}

				glPixelStorei(GL_UNPACK_ALIGNMENT, int(byteAlignment));
				ocean_assert(GL_NO_ERROR == glGetError());

				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, frame.constdata<void>(1u));
				ocean_assert(GL_NO_ERROR == glGetError());

				break;
			}

			case FrameType::FORMAT_Y_U_V12_LIMITED_RANGE:
			case FrameType::FORMAT_Y_U_V12_FULL_RANGE:
			case FrameType::FORMAT_Y_V_U12_LIMITED_RANGE:
			case FrameType::FORMAT_Y_V_U12_FULL_RANGE:
			{
				const bool uIsFirstPlane = frameType_.pixelFormat() == FrameType::FORMAT_Y_U_V12_LIMITED_RANGE
												|| frameType_.pixelFormat() == FrameType::FORMAT_Y_U_V12_FULL_RANGE;


				// we use the Y_U_V12 shader also for Y_V_U12, just switching the source planes
				const unsigned int firstPlaneIndex = uIsFirstPlane ? 1u : 2u;
				const unsigned int secondPlaneIndex = uIsFirstPlane ? 2u : 1u;

				const GLsizei height_2 = height / 2;

				rowLength = 0u;
				byteAlignment = 0u;
				if (!determineAlignment(frame.strideBytes(firstPlaneIndex), rowLength, byteAlignment))
				{
					return false;
				}

				glPixelStorei(GL_UNPACK_ALIGNMENT, int(byteAlignment));
				ocean_assert(GL_NO_ERROR == glGetError());

				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height_2, format, type, frame.constdata<void>(firstPlaneIndex));
				ocean_assert(GL_NO_ERROR == glGetError());

				rowLength = 0u;
				byteAlignment = 0u;
				if (!determineAlignment(frame.strideBytes(secondPlaneIndex), rowLength, byteAlignment))
				{
					return false;
				}

				glPixelStorei(GL_UNPACK_ALIGNMENT, int(byteAlignment));
				ocean_assert(GL_NO_ERROR == glGetError());

				const GLint& yOffset = GLint(height_2);

				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, yOffset, width, height_2, format, type, frame.constdata<void>(secondPlaneIndex));
				ocean_assert(GL_NO_ERROR == glGetError());

				break;
			}

			default:
				ocean_assert(false && "This should never happen!");
				return false;
		}
	}
	else
	{
		if (secondaryTextureId_ != 0u)
		{
			// previously we needed a second texture, now we do not need it anymore

			glDeleteTextures(1, &secondaryTextureId_);
			ocean_assert(GL_NO_ERROR == glGetError());
			secondaryTextureId_ = 0u;
		}
	}

	if (useMipmap_)
	{
		createMipmap();
	}

	return true;
}

}

}

}
