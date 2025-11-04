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

}

}

}
