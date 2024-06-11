/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESMediaTexture2D.h"
#include "ocean/rendering/glescenegraph/GLESAttributeSet.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/media/FrameMedium.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESMediaTexture2D::GLESMediaTexture2D() :
	GLESTexture(),
	Texture2D(),
	DynamicObject(),
	GLESTexture2D(),
	MediaTexture2D()
{
	// nothing to do here
}

GLESMediaTexture2D::~GLESMediaTexture2D()
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
}

std::string GLESMediaTexture2D::textureName() const
{
	const ScopedLock scopedLock(objectLock);

	return textureName_;
}

bool GLESMediaTexture2D::setTextureName(const std::string& name)
{
	if (name.empty())
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	textureName_ = name;

	return true;
}

bool GLESMediaTexture2D::isValid() const
{
	return primaryTextureId_ != 0u;
}

std::string GLESMediaTexture2D::descriptiveInformation() const
{
	const ScopedLock scopedLock(objectLock);

	std::string result = Object::descriptiveInformation();

	if (frameMedium_)
	{
		result += ", with medium '" + frameMedium_->url() + "'";
	}

	return result;
}

unsigned int GLESMediaTexture2D::bindTexture(GLESShaderProgram& shaderProgram, const unsigned int id)
{
	ocean_assert(GL_NO_ERROR == glGetError());

	const GLenum glesMinificationFilterMode = translateMinificationFilterMode(minificationFilterMode_);
	const GLenum glesMagnificationFilterMode = translateMagnificationFilterMode(magnificationFilterMode_);

	const GLenum glesWrapTypeS = translateWrapType(wrapTypeS_);
	const GLenum glesWrapTypeT = translateWrapType(wrapTypeT_);

	if (primaryTextureId_ != 0u)
	{
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

		if (secondaryTextureId_ != 0u)
		{
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
				setUniform(locationSecondaryTexture, int(id + 1u));
			}

			return 2u;
		}

		return 1u;
	}

	return 0u;
}

void GLESMediaTexture2D::createMipmap()
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

bool GLESMediaTexture2D::defineTextureObject(const FrameType& frameType)
{
	ocean_assert(frameType.isValid());

	return definePrimaryTextureObject(frameType) && defineSecondaryTextureObject(frameType);
}

bool GLESMediaTexture2D::definePrimaryTextureObject(const FrameType& frameType)
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
	}

	glBindTexture(GL_TEXTURE_2D, primaryTextureId_);
	ocean_assert(GL_NO_ERROR == glGetError());

#ifdef __APPLE__

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
			Log::warning() << "Due to the non-power-of-two texture the warp-s mode was set to clamp-to-edge!";
			wrapTypeS_ = WRAP_CLAMP;
		}

		if (wrapTypeT_ != WRAP_CLAMP)
		{
			Log::warning() << "Due to the non-power-of-two texture the warp-t mode was set to clamp-to-edge!";
			wrapTypeT_ = WRAP_CLAMP;
		}
	}

#endif

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, nullptr);
	ocean_assert(GL_NO_ERROR == glGetError());

	return true;
}

bool GLESMediaTexture2D::defineSecondaryTextureObject(const FrameType& frameType)
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
	}

	glBindTexture(GL_TEXTURE_2D, secondaryTextureId_);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, nullptr);
	ocean_assert(GL_NO_ERROR == glGetError());

	return true;
}

void GLESMediaTexture2D::onDynamicUpdate(const ViewRef& /*view*/, const Timestamp timestamp)
{
	const ScopedLock scopedLock(objectLock);

	if (frameMedium_.isNull())
	{
		return;
	}

	if (timestamp != renderTimestamp_)
	{
		const FrameRef frameRef = frameMedium_->frame(timestamp);
		if (frameRef.isNull())
		{
			return;
		}

		renderTimestamp_ = timestamp;

		const Frame& frame = *frameRef;

		if (frame && frame.timestamp() != frameTimestamp_)
		{
			FrameType internalFrameType;
			if (determineInternalFrameType(frame, internalFrameType))
			{
				ocean_assert(internalFrameType.isValid());

				if (internalFrameType != frameType_)
				{
					if (!defineTextureObject(internalFrameType))
					{
						ocean_assert(false && "This should never happen!");
						return;
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

				if (frame.frameType() == frameType_)
				{
					GLenum format, type;
					unsigned int width, height;

					if (determinePrimaryTextureProperties(frameType_, width, height, format, type))
					{
						ocean_assert(primaryTextureId_ != 0u);
						ocean_assert(GL_NO_ERROR == glGetError());

						glBindTexture(GL_TEXTURE_2D, primaryTextureId_);
						ocean_assert(GL_NO_ERROR == glGetError());

						unsigned int rowLength = 0u;
						unsigned int byteAlignment = 0u;
						if (determineAlignment(frame.strideBytes(0u), rowLength, byteAlignment))
						{
							glPixelStorei(GL_UNPACK_ALIGNMENT, int(byteAlignment));
							ocean_assert(GL_NO_ERROR == glGetError());

							glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, frame.constdata<void>(0u));
							ocean_assert(GL_NO_ERROR == glGetError());
						}
					}

					if (determineSecondaryTextureProperties(frameType_, width, height, format, type))
					{
						ocean_assert(secondaryTextureId_ != 0);
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
								unsigned int rowLength = 0u;
								unsigned int byteAlignment = 0u;
								if (determineAlignment(frame.strideBytes(1u), rowLength, byteAlignment))
								{
									glPixelStorei(GL_UNPACK_ALIGNMENT, int(byteAlignment));
									ocean_assert(GL_NO_ERROR == glGetError());

									glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, frame.constdata<void>(1u));
									ocean_assert(GL_NO_ERROR == glGetError());
								}

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

								unsigned int rowLength = 0u;
								unsigned int byteAlignment = 0u;
								if (determineAlignment(frame.strideBytes(firstPlaneIndex), rowLength, byteAlignment))
								{
									glPixelStorei(GL_UNPACK_ALIGNMENT, int(byteAlignment));
									ocean_assert(GL_NO_ERROR == glGetError());

									glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height_2, format, type, frame.constdata<void>(firstPlaneIndex));
									ocean_assert(GL_NO_ERROR == glGetError());
								}

								rowLength = 0u;
								byteAlignment = 0u;
								if (determineAlignment(frame.strideBytes(secondPlaneIndex), rowLength, byteAlignment))
								{
									glPixelStorei(GL_UNPACK_ALIGNMENT, int(byteAlignment));
									ocean_assert(GL_NO_ERROR == glGetError());

									const GLint& yOffset = GLint(height_2);

									glTexSubImage2D(GL_TEXTURE_2D, 0, 0, yOffset, width, height_2, format, type, frame.constdata<void>(secondPlaneIndex));
									ocean_assert(GL_NO_ERROR == glGetError());
								}

								break;
							}

							default:
								ocean_assert(false && "This should never happen!");
						}
					}
				}
				else
				{
					ocean_assert(secondaryTextureId_ == 0);

					GLenum format, type;
					unsigned int width, height;

					if (determinePrimaryTextureProperties(frameType_, width, height, format, type))
					{
						CV::FrameConverter::Options convertOptions;
						if (frame.pixelFormat() == FrameType::FORMAT_Y10_PACKED || frame.pixelFormat() == FrameType::FORMAT_RGGB10_PACKED)
						{
							ocean_assert(frameType_.pixelFormat() == FrameType::FORMAT_Y8 || frameType_.pixelFormat() == FrameType::FORMAT_RGB24);

							constexpr float gamma = 0.7f;
							convertOptions = CV::FrameConverter::Options(gamma);
						}

						if (CV::FrameConverter::Comfort::convert(frame, frameType_.pixelFormat(), frameType_.pixelOrigin(), conversionFrame_, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()(), convertOptions))
						{
							ocean_assert(conversionFrame_.numberPlanes() == 1u);
							ocean_assert(conversionFrame_.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

							ocean_assert(primaryTextureId_ != 0);
							ocean_assert(GL_NO_ERROR == glGetError());

							glBindTexture(GL_TEXTURE_2D, primaryTextureId_);
							ocean_assert(GL_NO_ERROR == glGetError());

							unsigned int rowLength = 0u;
							unsigned int byteAlignment = 0u;
							if (determineAlignment(conversionFrame_.strideBytes(), rowLength, byteAlignment))
							{
								glPixelStorei(GL_UNPACK_ALIGNMENT, int(byteAlignment));
								ocean_assert(GL_NO_ERROR == glGetError());

								glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, conversionFrame_.constdata<void>());
								ocean_assert(GL_NO_ERROR == glGetError());
							}
						}
					}
				}

				if (useMipmap_)
				{
					createMipmap();
				}
			}
		}
	}
}

bool GLESMediaTexture2D::needsSecondaryTextureObjects(const FrameType& frameType)
{
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

bool GLESMediaTexture2D::determineInternalFrameType(const FrameType& frameType, FrameType& internalFrameType)
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

bool GLESMediaTexture2D::determinePrimaryTextureProperties(const FrameType& frameType, unsigned int& width, unsigned int& height, GLenum& format, GLenum& type)
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

bool GLESMediaTexture2D::determineSecondaryTextureProperties(const FrameType& frameType, unsigned int& width, unsigned int& height, GLenum& format, GLenum& type)
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

bool GLESMediaTexture2D::primaryTextureName(const std::string& names, std::string& name)
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

bool GLESMediaTexture2D::secondaryTextureName(const std::string& names, std::string& name)
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
