// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/globalillumination/GIStripPrimitive.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GIStripPrimitive::GIStripPrimitive() :
	GIPrimitive(),
	StripPrimitive()
{
	// nothing to do here
}

GIStripPrimitive::~GIStripPrimitive()
{
	// nothing to do here
}

VertexIndexGroups GIStripPrimitive::strips() const
{
	return primitiveStrips;
}

}

}

}
