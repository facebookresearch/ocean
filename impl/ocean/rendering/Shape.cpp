/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Shape.h"

namespace Ocean
{

namespace Rendering
{

Shape::Shape() :
	Renderable()
{
	// nothing to do here
}

Shape::~Shape()
{
	// nothing to do here
}

Shape::ObjectType Shape::type() const
{
	return TYPE_SHAPE;
}

}

}
