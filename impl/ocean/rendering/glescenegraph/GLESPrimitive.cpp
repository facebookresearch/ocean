/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
