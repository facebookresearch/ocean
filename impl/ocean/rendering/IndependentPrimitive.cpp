// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/IndependentPrimitive.h"

namespace Ocean
{

namespace Rendering
{

IndependentPrimitive::IndependentPrimitive()
{
	// nothing to do here
}

IndependentPrimitive::~IndependentPrimitive()
{
	// nothing to do here
}

IndependentPrimitive::ObjectType IndependentPrimitive::type() const
{
	return TYPE_INDEPENDENT_PRIMITIVE;
}

}

}
