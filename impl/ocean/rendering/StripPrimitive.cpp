// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/StripPrimitive.h"

namespace Ocean
{

namespace Rendering
{

StripPrimitive::StripPrimitive() :
	Primitive()
{
	// nothing to do here
}

StripPrimitive::~StripPrimitive()
{
	// nothing to do here
}

VertexIndexGroups StripPrimitive::strips() const
{
	throw NotSupportedException("StripPrimitive::strips() is not supported.");
}

unsigned int StripPrimitive::numberStrips() const
{
	throw NotSupportedException("StripPrimitive::numberStrips() is not supported.");
}

void StripPrimitive::setStrips(const VertexIndexGroups& /*strips*/)
{
	throw NotSupportedException("StripPrimitive::setStrips() is not supported.");
}

StripPrimitive::ObjectType StripPrimitive::type() const
{
	return TYPE_STRIP_PRIMITIVE;
}

}

}
