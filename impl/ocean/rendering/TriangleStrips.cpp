/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
