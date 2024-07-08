/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESTexture2D.h"
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

GLESTexture2D::GLESTexture2D() :
	GLESTexture(),
	Texture2D(),
	DynamicObject(),
	wrapTypeS_(WRAP_CLAMP),
	wrapTypeT_(WRAP_CLAMP)
{
	registerDynamicUpdateObject();
}

GLESTexture2D::~GLESTexture2D()
{
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

}

}

}
