/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/TriangleFans.h"

namespace Ocean
{

namespace Rendering
{

TriangleFans::TriangleFans() :
	StripPrimitive()
{
	// nothing to do here
}

TriangleFans::~TriangleFans()
{
	// nothing to do here
}

TriangleFans::ObjectType TriangleFans::type() const
{
	return TYPE_TRIANGLE_FANS;
}


}

}
