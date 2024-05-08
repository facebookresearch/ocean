/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/IndependentPrimitive.h"

namespace Ocean
{

namespace Rendering
{

IndependentPrimitive::IndependentPrimitive()
{
	// nothing to do here
}

IndependentPrimitive::~IndependentPrimitive()
{
	// nothing to do here
}

IndependentPrimitive::ObjectType IndependentPrimitive::type() const
{
	return TYPE_INDEPENDENT_PRIMITIVE;
}

}

}
