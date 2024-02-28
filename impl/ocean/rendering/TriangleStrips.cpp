// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/TriangleStrips.h"

namespace Ocean
{

namespace Rendering
{

TriangleStrips::TriangleStrips() :
	Primitive()
{
	// nothing to do here
}

TriangleStrips::~TriangleStrips()
{
	// nothing to do here
}

TriangleStrips::ObjectType TriangleStrips::type() const
{
	return TYPE_TRIANGLE_STRIPS;
}


}

}
