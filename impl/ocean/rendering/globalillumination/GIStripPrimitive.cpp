/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
