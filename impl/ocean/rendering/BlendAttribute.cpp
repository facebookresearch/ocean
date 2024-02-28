// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
