// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/PhantomAttribute.h"

namespace Ocean
{

namespace Rendering
{

PhantomAttribute::PhantomAttribute() :
	Attribute()
{
	// nothing to do here
}

PhantomAttribute::ObjectType PhantomAttribute::type() const
{
	return TYPE_PHANTOM_ATTRIBUTE;
}

}

}
