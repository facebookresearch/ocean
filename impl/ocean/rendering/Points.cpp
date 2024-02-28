// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/Points.h"

namespace Ocean
{

namespace Rendering
{

Points::Points() :
	IndependentPrimitive()
{
	// nothing to do here
}

Points::~Points()
{
	// nothing to do here
}

VertexIndices Points::indices() const
{
	throw NotSupportedException();
}

unsigned int Points::numberIndices() const
{
	throw NotSupportedException();
}

Scalar Points::pointSize() const
{
	throw NotSupportedException();
}

void Points::setIndices(const VertexIndices& /*indices*/)
{
	throw NotSupportedException();
}

void Points::setIndices(const unsigned int /*numberImplicitPoints*/)
{
	throw NotSupportedException();
}

Points::ObjectType Points::type() const
{
	return TYPE_POINTS;
}

void Points::setPointSize(const Scalar /*size*/)
{
	throw NotSupportedException();
}

}

}
