/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Cone.h"

namespace Ocean
{

namespace Rendering
{

Cone::Cone() :
	Shape()
{
	// nothing to do here
}

Cone::~Cone()
{
	// nothing to do here
}

Scalar Cone::radius() const
{
	throw NotSupportedException("Cone radius is not supported.");
}

Scalar Cone::height() const
{
	throw NotSupportedException("Cone height is not supported.");
}

bool Cone::bottom() const
{
	throw NotSupportedException("Cone bottom is not supported.");
}

bool Cone::side() const
{
	throw NotSupportedException("Cone side is not supported.");
}

bool Cone::setRadius(const Scalar /*radius*/)
{
	throw NotSupportedException("Cone radius is not supported.");
}

bool Cone::setHeight(const Scalar /*height*/)
{
	throw NotSupportedException("Cone height is not supported.");
}

bool Cone::setSide(const bool /*visible*/)
{
	throw NotSupportedException("Cone side is not supported.");
}

bool Cone::setBottom(const bool /*visible*/)
{
	throw NotSupportedException("Cone bottom is not supported.");
}

Cone::ObjectType Cone::type() const
{
	return TYPE_CONE;
}

}

}
