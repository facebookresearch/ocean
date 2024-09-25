/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/LineStrips.h"

namespace Ocean
{

namespace Rendering
{

LineStrips::LineStrips() :
	Primitive()
{
	// nothing to do here
}

LineStrips::~LineStrips()
{
	// nothing to do here
}

LineStrips::ObjectType LineStrips::type() const
{
	return TYPE_LINE_STRIPS;
}

Scalar LineStrips::lineWidth() const
{
	throw NotSupportedException();
}

void LineStrips::setLineWidth(const Scalar /*width*/)
{
	throw NotSupportedException();
}

}

}
