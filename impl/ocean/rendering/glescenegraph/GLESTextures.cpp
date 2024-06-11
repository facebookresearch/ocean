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
	for (size_t n = 0; n < textures.size(); ++n)
	{
		const SmartObjectRef<GLESTexture> texture(textures[n]);
		ocean_assert(texture);

		texture->bindTexture(shaderProgram, (unsigned int)(n));
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

		if (texture2D->frameType().pixelFormat() == FrameType::FORMAT_UNDEFINED
				|| texture2D->frameType().pixelOrigin() == FrameType::ORIGIN_INVALID)
		{
			return GLESAttribute::PT_PENDING;
		}

		GLESAttribute::ProgramType type = GLESAttribute::PT_UNKNOWN;

		if (texture2D->frameType().pixelOrigin() == FrameType::ORIGIN_LOWER_LEFT)
		{
			type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_LOWER_LEFT);
		}
		else
		{
			type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_UPPER_LEFT);
		}

		switch (std::underlying_type<FrameType::PixelFormat>::type(texture2D->frameType().pixelFormat()))
		{
			case FrameType::FORMAT_BGR24:
			case FrameType::FORMAT_BGRA32:
				type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_BGRA);
				break;

			case FrameType::FORMAT_Y8:
				type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_Y);
				break;

			case FrameType::FORMAT_RGB24:
			case FrameType::FORMAT_RGBA32:
				type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_RGBA);
				break;

			case FrameType::FORMAT_Y_U_V24_LIMITED_RANGE: // we are currently using the same shader for limited and full value range, may have a minor impact on color precision
			case FrameType::FORMAT_Y_U_V24_FULL_RANGE:
				type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_YUV24);
				break;

			case FrameType::FORMAT_YVU24:
				type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_YVU24);
				break;

			case FrameType::FORMAT_Y_UV12_LIMITED_RANGE:
			case FrameType::FORMAT_Y_UV12_FULL_RANGE:
				type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_Y_UV12);
				break;

			case FrameType::FORMAT_Y_VU12_LIMITED_RANGE:
			case FrameType::FORMAT_Y_VU12_FULL_RANGE:
				type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_Y_VU12);
				break;

			case FrameType::FORMAT_Y_U_V12_LIMITED_RANGE:
			case FrameType::FORMAT_Y_U_V12_FULL_RANGE:
			case FrameType::FORMAT_Y_V_U12_LIMITED_RANGE:
			case FrameType::FORMAT_Y_V_U12_FULL_RANGE:
				type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_Y_U_V12);
				break;

			case FrameType::genericPixelFormat<float, 1u>():
				type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_Y);
				break;

			case FrameType::genericPixelFormat<float, 3u>():
			case FrameType::genericPixelFormat<float, 4u>():
				type = GLESAttribute::ProgramType(type | GLESAttribute::PT_TEXTURE_RGBA);
				break;

			default:
				ocean_assert(false && "Missing implementation!");
		}

		return type;
	}

	return GLESAttribute::PT_UNKNOWN;
}

}

}

}
