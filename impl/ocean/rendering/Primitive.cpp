// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/Primitive.h"

namespace Ocean
{

namespace Rendering
{

Primitive::Primitive() :
	Renderable()
{
	// nothing to do here
}

Primitive::~Primitive()
{
	unregisterThisObjectAsParent(primitiveVertexSet);
}

void Primitive::setVertexSet(const VertexSetRef& vertexSet)
{
	const ScopedLock scopedLock(objectLock);

	unregisterThisObjectAsParent(primitiveVertexSet);
	registerThisObjectAsParent(vertexSet);

	primitiveVertexSet = vertexSet;
}

Primitive::ObjectType Primitive::type() const
{
	return TYPE_PRIMITIVE;
}

}

}
