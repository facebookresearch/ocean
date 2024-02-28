// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/Transform.h"

namespace Ocean
{

namespace Rendering
{

Transform::Transform() :
	Group()
{
	// nothing to do here
}

Transform::~Transform()
{
	// nothing to do here
}

Transform::ObjectType Transform::type() const
{
	return TYPE_TRANSFORM;
}

}

}
