/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/PrimitiveAttribute.h"

namespace Ocean
{

namespace Rendering
{

PrimitiveAttribute::PrimitiveAttribute() :
	Attribute()
{
	// nothing to do here
}

PrimitiveAttribute::FaceMode PrimitiveAttribute::faceMode() const
{
	throw NotSupportedException("PrimitiveAttribute::faceMode() is not supported.");
}

PrimitiveAttribute::CullingMode PrimitiveAttribute::cullingMode() const
{
	throw NotSupportedException("PrimitiveAttribute::cullingMode() is not supported.");
}

PrimitiveAttribute::LightingMode PrimitiveAttribute::lightingMode() const
{
	throw NotSupportedException("PrimitiveAttribute::lightingMode() is not supported.");
}

void PrimitiveAttribute::setFaceMode(const FaceMode /*mode*/)
{
	throw NotSupportedException("PrimitiveAttribute::setFaceMode() is not supported.");
}

void PrimitiveAttribute::setCullingMode(const CullingMode /*culling*/)
{
	throw NotSupportedException("PrimitiveAttribute::setCullingMode() is not supported.");
}

void PrimitiveAttribute::setLightingMode(const LightingMode /*lighting*/)
{
	throw NotSupportedException("PrimitiveAttribute::setLightingMode() is not supported.");
}

PrimitiveAttribute::ObjectType PrimitiveAttribute::type() const
{
	return TYPE_PRIMITIVE_ATTRIBUTE;
}

}

}
