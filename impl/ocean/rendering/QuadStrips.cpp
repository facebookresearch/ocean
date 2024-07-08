/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/QuadStrips.h"

namespace Ocean
{

namespace Rendering
{

QuadStrips::QuadStrips() :
	StripPrimitive()
{
	// nothing to do here
}

QuadStrips::~QuadStrips()
{
	// nothing to do here
}

QuadStrips::ObjectType QuadStrips::type() const
{
	return TYPE_QUAD_STRIPS;
}

}

}
