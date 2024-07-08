/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESBackground.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESBackground::GLESBackground() :
	GLESNode(),
	Background(),
	backgroundPosition(0, 0, 0)
{
	// nothing to do here
}

GLESBackground::~GLESBackground()
{
	// nothing to do here
}

Vector3 GLESBackground::position() const
{
	return backgroundPosition;
}

Quaternion GLESBackground::orientation() const
{
	return backgroundOrientation;
}

void GLESBackground::setPosition(const Vector3& position)
{
	backgroundPosition = position;
}

void GLESBackground::setOrientation(const Quaternion& orientation)
{
	backgroundOrientation = orientation;
}

}

}

}
