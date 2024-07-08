/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
