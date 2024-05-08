/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/apple/GLESWindowFramebuffer.h"

#include "ocean/rendering/glescenegraph/GLESScene.h"
#include "ocean/rendering/glescenegraph/GLESUndistortedBackground.h"
#include "ocean/rendering/glescenegraph/GLESView.h"

#include "ocean/platform/apple/Utilities.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Apple
{

GLESWindowFramebuffer::GLESWindowFramebuffer() :
	GLESFramebuffer(),
	WindowFramebuffer(),
	device_T_display_(true)
{
	initialize();

	int zAxisDegree;
	if (Platform::Apple::Utilities::hasFixedSupportedInterfaceOrientation(zAxisDegree))
	{
		device_T_display_ = HomogenousMatrix4(Quaternion(Vector3(0, 0, 1), Numeric::deg2rad(Scalar(zAxisDegree))));
	}
}

GLESWindowFramebuffer::~GLESWindowFramebuffer()
{
	release();
}

bool GLESWindowFramebuffer::initializeContext()
{
	// nothing to do here
	return true;
}

HomogenousMatrix4 GLESWindowFramebuffer::device_T_display() const
{
	return device_T_display_;
}

bool GLESWindowFramebuffer::setDevice_T_display(const HomogenousMatrix4& device_T_display)
{
	if (device_T_display.isValid())
	{
		device_T_display_ = device_T_display;
		return true;
	}

	return false;
}

}

}

}

}
