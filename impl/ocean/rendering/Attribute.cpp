// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/Attribute.h"

namespace Ocean
{

namespace Rendering
{

Attribute::Attribute() :
	Object()
{
	// nothing to do here
}

Attribute::~Attribute()
{
	// nothing to do here
}

Attribute::ObjectType Attribute::type() const
{
	return TYPE_ATTRIBUTE;
}

}

}
