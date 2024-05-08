/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/BlendAttribute.h"

namespace Ocean
{

namespace Rendering
{

BlendAttribute::BlendAttribute() :
	Attribute()
{
	// nothing to do here
}

BlendAttribute::BlendingFunction BlendAttribute::sourceFunction() const
{
	throw NotSupportedException("Blend attribute source function is not supported.");
}

BlendAttribute::BlendingFunction BlendAttribute::destinationFunction() const
{
	throw NotSupportedException("Blend attribute destination function is not supported.");
}

void BlendAttribute::setSourceFunction(const BlendingFunction /*function*/)
{
	throw NotSupportedException("Blend attribute source function is not supported.");
}

void BlendAttribute::setDestinationFunction(const BlendingFunction /*function*/)
{
	throw NotSupportedException("Blend attribute destination function is not supported.");
}

BlendAttribute::ObjectType BlendAttribute::type() const
{
	return TYPE_BLEND_ATTRIBUTE;
}

}

}
