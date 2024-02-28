// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
