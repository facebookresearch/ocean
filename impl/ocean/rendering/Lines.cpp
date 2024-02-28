// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
