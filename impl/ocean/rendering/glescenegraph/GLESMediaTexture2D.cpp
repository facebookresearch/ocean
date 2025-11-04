/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESMediaTexture2D.h"
#include "ocean/rendering/glescenegraph/GLESAttributeSet.h"

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
	// nothing to do here
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

void GLESMediaTexture2D::onDynamicUpdate(const ViewRef& /*view*/, const Timestamp timestamp)
{
	const ScopedLock scopedLock(objectLock);

	if (frameMedium_.isNull())
	{
		return;
	}

	if (timestamp != renderTimestamp_)
	{
		const FrameRef frame = frameMedium_->frame(timestamp);
		if (frame.isNull())
		{
			return;
		}

		renderTimestamp_ = timestamp;

		if (frame->isValid() && frame->timestamp() != frameTimestamp_)
		{
			updateTexture(*frame);
		}
	}
}

bool GLESMediaTexture2D::updateTexture(const Frame& frame)
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
