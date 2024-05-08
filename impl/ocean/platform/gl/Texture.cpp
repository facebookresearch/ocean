/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/gl/Texture.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

Texture::Texture() :
	textureId(0u),
	textureFormat(0u)
{
	// nothing to do here
}

Texture::Texture(const Frame& frame) :
	textureId(0u),
	textureFormat(0u)
{
	update(frame);
}

Texture::~Texture()
{
	release();
}

bool Texture::resize(const FrameType& frameType, const bool enableFiltering)
{
	ocean_assert(associatedContext_);
	if (!associatedContext_)
	{
		return false;
	}

	ocean_assert(glGetError() == GL_NO_ERROR);

	if (textureFrameType != frameType)
	{
		GLint internalFormat = 0;
		const GLenum format = pixelFormat2textureFormat(frameType.pixelFormat(), &internalFormat);

		if (format == 0u || frameType.pixels() == 0u)
		{
			release();
			return false;
		}

		if (textureId == 0u)
		{
			glGenTextures(1, &textureId);
			ocean_assert(glGetError() == GL_NO_ERROR);

			if (textureId == 0u)
			{
				return false;
			}

			glBindTexture(GL_TEXTURE_2D, textureId);
			ocean_assert(glGetError() == GL_NO_ERROR);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, enableFiltering ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
			ocean_assert(glGetError() == GL_NO_ERROR);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, enableFiltering ? GL_LINEAR : GL_NEAREST);
			ocean_assert(glGetError() == GL_NO_ERROR);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			ocean_assert(glGetError() == GL_NO_ERROR);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			ocean_assert(glGetError() == GL_NO_ERROR);

			if (enableFiltering && associatedContext_->isExtensionSupported("GL_EXT_texture_filter_anisotropic"))
			{
				GLfloat largestValue = 0;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largestValue);
				ocean_assert(glGetError() == GL_NO_ERROR);

				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largestValue);
				ocean_assert(glGetError() == GL_NO_ERROR);
			}
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, textureId);
			ocean_assert(glGetError() == GL_NO_ERROR);
		}

		// we create the texture object but do not upload the frame data
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, frameType.width(), frameType.height(), 0, format, GL_UNSIGNED_BYTE, nullptr);
		ocean_assert(glGetError() == GL_NO_ERROR);

		textureFormat = format;
		textureFrameType = frameType;
	}

	ocean_assert(textureId != 0u);
	return true;
}

bool Texture::update(const Frame& frame, const bool enableFiltering)
{
	if (!resize(frame, enableFiltering))
	{
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, textureId);
	ocean_assert(glGetError() == GL_NO_ERROR);

	ocean_assert(frame.paddingElements() == 0u);
	if (frame.paddingElements() != 0u)
	{
		return false;
	}

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.width(), frame.height(), textureFormat, GL_UNSIGNED_BYTE, frame.constdata<void>());
	ocean_assert(glGetError() == GL_NO_ERROR);

	if (enableFiltering)
	{
		updateMipmap();
	}

	return true;
}

void Texture::updateMipmap()
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	ocean_assert(glGetError() == GL_NO_ERROR);

	associatedContext_->glGenerateMipmap(GL_TEXTURE_2D);
	ocean_assert(glGetError() == GL_NO_ERROR);
}

bool Texture::bindToProgram(const GLuint programId, const std::string& attributeName, const unsigned int index)
{
	ocean_assert(associatedContext_);
	if (!associatedContext_)
	{
		return false;
	}

	ocean_assert(GL_NO_ERROR == glGetError());

	ocean_assert(textureId != 0u);
	ocean_assert(programId != 0u);
	ocean_assert(index < GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
	ocean_assert(!attributeName.empty());

	ocean_assert(associatedContext_->glIsProgram(programId));
	associatedContext_->glUseProgram(programId);
	ocean_assert(GL_NO_ERROR == glGetError());

	associatedContext_->glActiveTexture(GLenum(GL_TEXTURE0 + index));
	ocean_assert(GL_NO_ERROR == glGetError());

	glBindTexture(GL_TEXTURE_2D, textureId);
	ocean_assert(GL_NO_ERROR == glGetError());

	const GLint location = associatedContext_->glGetUniformLocation(programId, attributeName.c_str());
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());

	associatedContext_->glUniform1i(location, GLint(index));
	ocean_assert(GL_NO_ERROR == glGetError());

	return true;
}

void Texture::release()
{
	if (textureId != 0u)
	{
		ocean_assert(glGetError() == GL_NO_ERROR);
		glDeleteTextures(1, &textureId);
		ocean_assert(glGetError() == GL_NO_ERROR);

		textureId = 0u;
	}

	textureFormat = 0u;
	textureFrameType = FrameType();
}

GLenum Texture::pixelFormat2textureFormat(const FrameType::PixelFormat pixelFormat, GLint* internalFormat)
{
	switch (pixelFormat)
	{
		case FrameType::FORMAT_BGR24:
		{
			if (internalFormat)
			{
				*internalFormat = GL_RGB8;
			}

			return GL_BGR;
		}

		case FrameType::FORMAT_BGRA32:
		{
			if (internalFormat)
			{
				*internalFormat = GL_RGBA8;
			}

			return GL_BGRA;
		}

		case FrameType::FORMAT_RGB24:
		{
			if (internalFormat)
			{
				*internalFormat = GL_RGB8;
			}

			return GL_RGB;
		}

		case FrameType::FORMAT_RGBA32:
		{
			if (internalFormat)
			{
				*internalFormat = GL_RGBA8;
			}

			return GL_RGBA;
		}

		case FrameType::FORMAT_Y8:
		case FrameType::FORMAT_Y_U_V12:
		case FrameType::FORMAT_Y_V_U12:
		case FrameType::FORMAT_Y_UV12:
		case FrameType::FORMAT_Y_VU12:
		{
			if (internalFormat)
			{
				*internalFormat = GL_LUMINANCE;
			}

			return GL_LUMINANCE;
		}

		case FrameType::FORMAT_YA16:
		{
			if (internalFormat)
			{
				*internalFormat = GL_LUMINANCE_ALPHA;
			}

			return GL_LUMINANCE_ALPHA;
		}

		// **NOTE** GL_LUMINANCE and GL_LUMINANCE_ALPHA are deprecated formats that have been removed from core OpenGL 3.1 and textures with this image format cannot be bound to FBOs.
		// Replace GL_LUMINANCE with GL_R8 and GL_LUMINANCE_ALPHA with GL_RG8. Shaders need to be modified accordingly.

		default:
			break;
	}

	ocean_assert(false && "Invalid pixel format!");
	return 0u;
}

}

}

}
