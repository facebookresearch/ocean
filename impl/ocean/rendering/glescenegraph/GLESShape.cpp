// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/glescenegraph/GLESShape.h"

#include "ocean/rendering/Engine.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESShape::GLESShape() :
	GLESRenderable(),
	Shape()
{
	shapeVertexSet = engine().factory().createVertexSet();
}

GLESShape::~GLESShape()
{
	// nothing to do here
}

}

}

}
