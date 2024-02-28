// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
