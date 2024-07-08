/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GITexture2D.h"


namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GITexture2D::GITexture2D() :
	GITexture(),
	Texture2D(),
	DynamicObject(),
	wrapTypeS_(WRAP_CLAMP),
	wrapTypeT_(WRAP_CLAMP)
{
	registerDynamicUpdateObject();
}

GITexture2D::~GITexture2D()
{
	unregisterDynamicUpdateObject();
}

GITexture2D::WrapType GITexture2D::wrapTypeS() const
{
	return wrapTypeS_;
}

GITexture2D::WrapType GITexture2D::wrapTypeT() const
{
	return wrapTypeT_;
}

bool GITexture2D::setWrapTypeS(const WrapType type)
{
	const ScopedLock scopedLock(objectLock);

	wrapTypeS_ = type;
	return true;
}

bool GITexture2D::setWrapTypeT(const WrapType type)
{
	const ScopedLock scopedLock(objectLock);

	wrapTypeT_ = type;
	return true;
}

}

}

}
