/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESStripPrimitive.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESStripPrimitive::GLESStripPrimitive() :
	GLESPrimitive(),
	StripPrimitive()
{
	// nothing to do here
}

GLESStripPrimitive::~GLESStripPrimitive()
{
	// nothing to do here
}

VertexIndexGroups GLESStripPrimitive::strips() const
{
	throw NotSupportedException("OpenGL ES does not support reading of vertex buffer objects.");
}

}

}

}
