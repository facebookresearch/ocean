// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/Box.h"

namespace Ocean
{

namespace Rendering
{

Box::Box() :
	Shape()
{
	// nothing to do here
}

Box::~Box()
{
	// nothing to do here
}

Vector3 Box::size() const
{
	throw NotSupportedException("Box size is not supported.");
}

bool Box::setSize(const Vector3& /*size*/)
{
	throw NotSupportedException("Box size is not supported.");
}

Box::ObjectType Box::type() const
{
	return TYPE_BOX;
}

}

}
