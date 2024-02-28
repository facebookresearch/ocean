// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/glescenegraph/GLESPrimitive.h"
#include "ocean/rendering/glescenegraph/GLESEngine.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESPrimitive::GLESPrimitive() :
	GLESRenderable(),
	Primitive()
{
	// nothing to do here
}

GLESPrimitive::~GLESPrimitive()
{
	// nothing to do here
}

void GLESPrimitive::setVertexSet(const VertexSetRef& vertexSet)
{
	Primitive::setVertexSet(vertexSet);

	updateBoundingBox();
}

}

}

}
