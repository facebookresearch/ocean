/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESTexture.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESTexture::GLESTexture() :
	GLESObject(),
	Texture(),
	transformation_(true),
	environmentMode_(MODE_REPLACE),
	minificationFilterMode_(MIN_MODE_LINEAR),
	magnificationFilterMode_(MAG_MODE_LINEAR),
	useMipmap_(false)
{
	// nothing to do here
}

GLESTexture::~GLESTexture()
{
	// nothing to do here
}

HomogenousMatrix4 GLESTexture::transformation() const
{
	const ScopedLock scopedLock(objectLock);

	return transformation_;
}

GLESTexture::EnvironmentMode GLESTexture::environmentMode() const
{
	const ScopedLock scopedLock(objectLock);

	return environmentMode_;
}

GLESTexture::MinFilterMode GLESTexture::minificationFilterMode() const
{
	const ScopedLock scopedLock(objectLock);

	return minificationFilterMode_;
}

GLESTexture::MagFilterMode GLESTexture::magnificationFilterMode() const
{
	const ScopedLock scopedLock(objectLock);

	return magnificationFilterMode_;
}

bool GLESTexture::usingMipmaps() const
{
	const ScopedLock scopedLock(objectLock);

	return useMipmap_;
}

void GLESTexture::setTransformation(const HomogenousMatrix4& transformation)
{
	const ScopedLock scopedLock(objectLock);

	ocean_assert(transformation.isValid());
	transformation_ = transformation;
}

void GLESTexture::setEnvironmentMode(const EnvironmentMode mode)
{
	const ScopedLock scopedLock(objectLock);

	environmentMode_ = mode;
}

void GLESTexture::setMinificationFilterMode(const MinFilterMode mode)
{
	const ScopedLock scopedLock(objectLock);

	minificationFilterMode_ = mode;
}

void GLESTexture::setMagnificationFilterMode(const MagFilterMode mode)
{
	const ScopedLock scopedLock(objectLock);

	magnificationFilterMode_ = mode;
}

void GLESTexture::setUseMipmaps(const bool flag)
{
	const ScopedLock scopedLock(objectLock);

	if (useMipmap_ == flag)
	{
		return;
	}

	useMipmap_ = flag;

	if (useMipmap_)
	{
		createMipmap();
	}
}

GLenum GLESTexture::translateMinificationFilterMode(const MinFilterMode minificationFilterMode)
{
	switch (minificationFilterMode)
	{
		case MIN_MODE_LINEAR:
			return GL_LINEAR;

		case MIN_MODE_NEAREST:
			return GL_NEAREST;

		case MIN_MODE_NEAREST_MIPMAP_NEAREST:
			return GL_NEAREST_MIPMAP_NEAREST;

		case MIN_MODE_LINEAR_MIPMAP_NEAREST:
			return GL_LINEAR_MIPMAP_NEAREST;

		case MIN_MODE_NEAREST_MIPMAP_LINEAR:
			return GL_NEAREST_MIPMAP_LINEAR;

		case MIN_MODE_LINEAR_MIPMAP_LINEAR:
			return GL_LINEAR_MIPMAP_LINEAR;

		case MIN_MODE_INVALID:
			break;
	}

	ocean_assert(false && "Invalid mode!");
	return GL_NEAREST;
}

GLenum GLESTexture::translateMagnificationFilterMode(const MagFilterMode magnificationFilterMode)
{
	switch (magnificationFilterMode)
	{
		case MAG_MODE_NEAREST:
			return GL_NEAREST;

		case MAG_MODE_LINEAR:
			return GL_LINEAR;

		case MAG_MODE_INVALID:
			break;
	}

	ocean_assert(false && "Invalid mode!");
	return GL_NEAREST;
}

GLenum GLESTexture::translateWrapType(const WrapType wrapType)
{
	switch (wrapType)
	{
		case WRAP_CLAMP:
			return GL_CLAMP_TO_EDGE;

		case WRAP_REPEAT:
			return GL_REPEAT;

		case WRAP_INVALID:
			break;
	}

	ocean_assert(false && "Invalid parameter");
	return WRAP_CLAMP;
}

}

}

}
