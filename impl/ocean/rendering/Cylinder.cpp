/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Cylinder.h"

namespace Ocean
{

namespace Rendering
{

Cylinder::Cylinder() :
	Shape()
{
	// nothing to do here
}

Cylinder::~Cylinder()
{
	// nothing to do here
}

Scalar Cylinder::height() const
{
	throw NotSupportedException("Cylinder height is not supported.");
}

Scalar Cylinder::radius() const
{
	throw NotSupportedException("Cylinder radius is not supported.");
}

bool Cylinder::top() const
{
	throw NotSupportedException("Cylinder top is not supported.");
}

bool Cylinder::bottom() const
{
	throw NotSupportedException("Cylinder bottom is not supported.");
}

bool Cylinder::side() const
{
	throw NotSupportedException("Cylinder side is not supported.");
}

bool Cylinder::setHeight(const Scalar /*height*/)
{
	throw NotSupportedException("Cylinder::setHeight() is not supported.");
}

bool Cylinder::setRadius(const Scalar /*radius*/)
{
	throw NotSupportedException("Cylinder::setRadius() is not supported.");
}

bool Cylinder::setTop(const bool /*visible*/)
{
	throw NotSupportedException("Cylinder::setTop() is not supported.");
}

bool Cylinder::setSide(const bool /*visible*/)
{
	throw NotSupportedException("Cylinder::setSide() is not supported.");
}

bool Cylinder::setBottom(const bool /*visible*/)
{
	throw NotSupportedException("Cylinder::setBottom() is not supported.");
}

Cylinder::ObjectType Cylinder::type() const
{
	return TYPE_CYLINDER;
}

}

}
