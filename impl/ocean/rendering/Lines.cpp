/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Lines.h"

namespace Ocean
{

namespace Rendering
{

Lines::Lines() :
	IndependentPrimitive()
{
	// nothing to do here
}

Lines::~Lines()
{
	// nothing to do here
}

VertexIndices Lines::indices() const
{
	throw NotSupportedException();
}

unsigned int Lines::numberIndices() const
{
	throw NotSupportedException();
}

void Lines::setIndices(const VertexIndices& /*indices*/)
{
	throw NotSupportedException();
}

void Lines::setIndices(const unsigned int /*numberImplicitPoints*/)
{
	throw NotSupportedException();
}

Lines::ObjectType Lines::type() const
{
	return TYPE_LINES;
}

}

}
