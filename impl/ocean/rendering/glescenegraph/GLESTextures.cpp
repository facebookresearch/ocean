/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESTextures.h"
#include "ocean/rendering/glescenegraph/GLESTexture.h"
#include "ocean/rendering/glescenegraph/GLESTexture2D.h"


namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESTextures::GLESTextures() :
	GLESAttribute(),
	Textures()
{
	// nothing to do here
}

GLESTextures::~GLESTextures()
{
	// nothing to do here
}

void GLESTextures::setTexture(const TextureRef& texture, const unsigned int layerIndex)
{
	const ScopedLock scopedLock(objectLock);
	Textures::setTexture(texture, layerIndex);
}

void GLESTextures::bindAttribute(const GLESFramebuffer& /*framebuffer*/, GLESShaderProgram& shaderProgram)
{
	for (size_t nTexture = 0; nTexture < textures_.size(); ++nTexture)
	{
		const SmartObjectRef<GLESTexture> texture(textures_[nTexture]);
		ocean_assert(texture);

		texture->bindTexture(shaderProgram, (unsigned int)(nTexture));
	}
}

void GLESTextures::unbindAttribute()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	ocean_assert(GL_NO_ERROR == glGetError());
}

GLESAttribute::ProgramType GLESTextures::necessaryShader() const
{
	if (numberTextures() > 1u)
	{
		return GLESAttribute::PT_TEXTURES;
	}

	if (numberTextures() == 1u)
	{
		const TextureRef firstTexture = texture(0);
		ocean_assert(firstTexture);

		if (firstTexture->type() == TYPE_TEXTURE_FRAMEBUFFER)
		{
			return GLESAttribute::ProgramType(GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_RGBA);
		}

		const SmartObjectRef<GLESTexture2D> texture2D(firstTexture);
		ocean_assert(texture2D);

		const FrameType frameType = texture2D->frameType();

		if (frameType.pixelFormat() == FrameType::FORMAT_UNDEFINED
				|| frameType.pixelOrigin() == FrameType::ORIGIN_INVALID)
		{
			return GLESAttribute::PT_PENDING;
		}

		GLESAttribute::ProgramType type = GLESAttribute::PT_UNKNOWN;

		if (frameType.pixelOrigin() == FrameType::ORIGIN_LOWER_LEFT)
		{
			type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_LOWER_LEFT);
		}
		else
		{
			type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_UPPER_LEFT);
		}

		// the shader needs to match the data which is uploaded, which can use a different pixel format than the frame
		const GLESTexture2D::TextureProperties* properties = GLESTexture2D::textureProperties(frameType.pixelFormat());

		if (properties == nullptr || properties->programType_ == GLESAttribute::PT_UNKNOWN)
		{
			ocean_assert(false && "Missing implementation!");
			return type;
		}

		type = GLESAttribute::ProgramType(type | properties->programType_);

		return type;
	}

	return GLESAttribute::PT_UNKNOWN;
}

}

}

}
